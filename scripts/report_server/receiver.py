"""DRAFT, not deployed: receives the crash, freeze and out-of-sync reports the game sends.

The game POSTs a zip (Content-Type: application/zip) with a one-line JSON note in the
X-OpenYR-Report header: {"format":1,"id":...,"kind":"crash|hang|desync","commit":...,"time":...,
"session":...}. This server checks the size, the sender's rate and the zip, stores the report on
disk, and leaves a notice in the notify folder for notify.py to post to the staff channel.

It speaks plain HTTP on the loopback address. HTTPS is the job of the web server in front of it
(see README.md), which must pass the sender's address in X-Forwarded-For.

  python3 -I receiver.py --root /var/lib/openyr-reports [--port 8471]
"""

import argparse
import hashlib
import http.server
import io
import json
import pathlib
import re
import threading
import time
import zipfile

# The game never sends more than 16 MB of files; the zip adds a little to that.
MAX_BODY_BYTES = 17 * 1024 * 1024
# A zip may hold at most this many files, unpacking to at most this much.
MAX_FILES = 32
MAX_UNPACKED_BYTES = 64 * 1024 * 1024
# Each sender address may send this many reports an hour.
REPORTS_PER_HOUR = 10
KINDS = ('crash', 'hang', 'desync')
ID_PATTERN = re.compile(r'^[0-9]{8}-[0-9]{6}-(crash|hang|desync)-[0-9]+-[0-9]+$')
NAME_PATTERN = re.compile(r'^[A-Za-z0-9._ -]{1,128}$')


class ReportError(Exception):
    """A report refused, with the HTTP status to answer with."""

    def __init__(self, status, reason):
        super().__init__(reason)
        self.status = status
        self.reason = reason


class RateLimiter:
    """Allows each address so many reports in a sliding hour."""

    def __init__(self, per_hour=REPORTS_PER_HOUR, clock=time.monotonic):
        self.per_hour = per_hour
        self.clock = clock
        self.sent = {}
        self.lock = threading.Lock()

    def allow(self, address):
        now = self.clock()
        with self.lock:
            recent = [t for t in self.sent.get(address, []) if now - t < 3600]
            if len(recent) >= self.per_hour:
                self.sent[address] = recent
                return False
            recent.append(now)
            self.sent[address] = recent
            return True


def parse_meta(text):
    """Reads and checks the note from the X-OpenYR-Report header."""
    try:
        meta = json.loads(text or '')
    except ValueError:
        raise ReportError(400, 'the report note is not JSON')
    if not isinstance(meta, dict) or meta.get('format') != 1:
        raise ReportError(400, 'the report note is not format 1')
    for key in ('id', 'kind', 'commit', 'time'):
        if not isinstance(meta.get(key), str):
            raise ReportError(400, 'the report note has no ' + key)
    if meta['kind'] not in KINDS:
        raise ReportError(400, 'unknown report kind')
    if not ID_PATTERN.match(meta['id']) or meta['id'].split('-')[2] != meta['kind']:
        raise ReportError(400, 'bad report id')
    if len(meta['commit']) > 64 or len(meta['time']) > 32 or len(meta.get('session', '')) > 256:
        raise ReportError(400, 'the report note is too long')
    return meta


def check_zip(data):
    """Opens the zip and checks every entry, returning the entry names."""
    try:
        archive = zipfile.ZipFile(io.BytesIO(data))
    except zipfile.BadZipFile:
        raise ReportError(400, 'not a zip')
    with archive:
        entries = archive.infolist()
        if not entries or len(entries) > MAX_FILES:
            raise ReportError(400, 'wrong number of files')
        if sum(entry.file_size for entry in entries) > MAX_UNPACKED_BYTES:
            raise ReportError(400, 'unpacks too big')
        for entry in entries:
            # Plain names only: no folders, so nothing can be written outside its report.
            if not NAME_PATTERN.match(entry.filename) or entry.filename.startswith('.'):
                raise ReportError(400, 'bad file name in the zip')
        if archive.testzip() is not None:
            raise ReportError(400, 'the zip is damaged')
        return [entry.filename for entry in entries]


def session_folder_name(session):
    """The folder every seat's out-of-sync report from one match goes in."""
    return hashlib.sha256(session.encode('utf-8')).hexdigest()[:16]


def store(root, meta, data, names, address):
    """Writes the report under root and leaves a notice for notify.py. Returns the folder, and
    whether it was new: a game whose answer was lost sends the same report again."""
    root = pathlib.Path(root)
    month = meta['time'][:7] if re.match(r'^[0-9]{4}-[0-9]{2}', meta['time']) else 'unknown'
    if meta['kind'] == 'desync' and meta.get('session'):
        folder = root / 'desync' / 'sessions' / session_folder_name(meta['session']) / meta['id']
    else:
        folder = root / meta['kind'] / month / meta['id']
    if folder.exists():
        return folder, False
    folder.mkdir(parents=True)
    (folder / 'report.zip').write_bytes(data)
    received = dict(meta, files=names, bytes=len(data), received=time.strftime('%Y-%m-%dT%H:%M:%SZ', time.gmtime()),
                    sender=hashlib.sha256(address.encode('utf-8')).hexdigest()[:12])
    (folder / 'meta.json').write_text(json.dumps(received, indent=1), encoding='utf-8')

    seats = 1
    if meta['kind'] == 'desync' and meta.get('session'):
        seats = sum(1 for p in folder.parent.iterdir() if p.is_dir())
    notice = dict(received, folder=folder.relative_to(root).as_posix(), seats=seats)
    notify = root / 'notify'
    notify.mkdir(exist_ok=True)
    temporary = notify / (meta['id'] + '.tmp')
    temporary.write_text(json.dumps(notice), encoding='utf-8')
    temporary.replace(notify / (meta['id'] + '.json'))
    return folder, True


def accept(root, limiter, address, content_type, length, meta_text, read_body):
    """Runs every check on one request and stores it. read_body(n) reads the body. Returns what
    store() returns."""
    if content_type != 'application/zip':
        raise ReportError(415, 'send application/zip')
    if length is None or length <= 0:
        raise ReportError(411, 'no length')
    if length > MAX_BODY_BYTES:
        raise ReportError(413, 'too big')
    meta = parse_meta(meta_text)
    if not limiter.allow(address):
        raise ReportError(429, 'too many reports, try later')
    data = read_body(length)
    if len(data) != length:
        raise ReportError(400, 'the body broke off')
    names = check_zip(data)
    return store(root, meta, data, names, address)


class Handler(http.server.BaseHTTPRequestHandler):
    server_version = 'OpenYRReports/0'

    def do_POST(self):
        if self.path != '/report':
            self.answer(404, 'no such page')
            return
        # The web server in front puts the sender last in X-Forwarded-For.
        forwarded = self.headers.get('X-Forwarded-For', '')
        address = forwarded.split(',')[-1].strip() or self.client_address[0]
        try:
            length = int(self.headers.get('Content-Length', ''))
        except ValueError:
            length = None
        try:
            folder, new = accept(self.server.root, self.server.limiter, address, self.headers.get('Content-Type', ''),
                            length, self.headers.get('X-OpenYR-Report', ''), self.rfile.read)
        except ReportError as error:
            self.answer(error.status, error.reason)
            return
        if not new:
            # Already stored: the game counts this as sent and stops resending it.
            self.answer(200, 'already have it')
            return
        self.log_message('stored %s', folder)
        self.answer(201, 'thank you')

    def answer(self, status, text):
        body = (text + '\n').encode('utf-8')
        self.send_response(status)
        self.send_header('Content-Type', 'text/plain; charset=utf-8')
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--root', required=True, help='where reports are stored')
    parser.add_argument('--port', type=int, default=8471)
    args = parser.parse_args()
    server = http.server.ThreadingHTTPServer(('127.0.0.1', args.port), Handler)
    server.root = args.root
    server.limiter = RateLimiter()
    server.serve_forever()


if __name__ == '__main__':
    main()
