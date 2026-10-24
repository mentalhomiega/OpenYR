"""Checks receiver.py and notify.py without a network: every check is called directly.

  python3 -I -m unittest discover -s scripts/report_server
"""

import io
import json
import pathlib
import sys
import tempfile
import unittest
import zipfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import notify  # noqa: E402
import receiver  # noqa: E402


def make_zip(files):
    buffer = io.BytesIO()
    with zipfile.ZipFile(buffer, 'w', zipfile.ZIP_STORED) as archive:
        for name, data in files.items():
            archive.writestr(name, data)
    return buffer.getvalue()


def note(kind='crash', id_=None, session=None):
    meta = {'format': 1, 'id': id_ or '20261024-010203-%s-1234-1' % kind, 'kind': kind,
            'commit': '6d60b4a9', 'time': '2026-10-24T01:02:03Z'}
    if session is not None:
        meta['session'] = session
    return json.dumps(meta)


class MetaTests(unittest.TestCase):
    def test_good_note(self):
        self.assertEqual(receiver.parse_meta(note())['kind'], 'crash')

    def test_refuses_bad_notes(self):
        for text in ['', 'not json', '[]', json.dumps({'format': 2}),
                     note(kind='oops', id_='20261024-010203-oops-1-1'),
                     note(id_='../../etc-passwd'),
                     note(kind='hang', id_='20261024-010203-crash-1-1')]:
            with self.assertRaises(receiver.ReportError, msg=text):
                receiver.parse_meta(text)


class ZipTests(unittest.TestCase):
    def test_good_zip(self):
        self.assertEqual(receiver.check_zip(make_zip({'except.txt': 'x', 'minidump.dmp': b'\0' * 10})),
                         ['except.txt', 'minidump.dmp'])

    def test_refuses_bad_zips(self):
        for data in [b'not a zip', make_zip({}), make_zip({'../evil.txt': 'x'}), make_zip({'sub/file.txt': 'x'}),
                     make_zip({'.hidden': 'x'}), make_zip({'f%d.txt' % i: 'x' for i in range(receiver.MAX_FILES + 1)})]:
            with self.assertRaises(receiver.ReportError):
                receiver.check_zip(data)


class RateTests(unittest.TestCase):
    def test_limit_per_hour(self):
        now = [0.0]
        limiter = receiver.RateLimiter(per_hour=2, clock=lambda: now[0])
        self.assertTrue(limiter.allow('a'))
        self.assertTrue(limiter.allow('a'))
        self.assertFalse(limiter.allow('a'))
        self.assertTrue(limiter.allow('b'))
        now[0] = 3601.0
        self.assertTrue(limiter.allow('a'))


class AcceptTests(unittest.TestCase):
    def setUp(self):
        self.folder = tempfile.TemporaryDirectory()
        self.root = pathlib.Path(self.folder.name)
        self.limiter = receiver.RateLimiter()

    def tearDown(self):
        self.folder.cleanup()

    def send(self, meta, data, content_type='application/zip', length=None):
        stream = io.BytesIO(data)
        return receiver.accept(self.root, self.limiter, '192.0.2.1', content_type,
                               len(data) if length is None else length, meta, stream.read)

    def test_stores_a_crash(self):
        folder, new = self.send(note(), make_zip({'except.txt': 'boom'}))
        self.assertTrue(new)
        self.assertTrue((folder / 'report.zip').is_file())
        stored = json.loads((folder / 'meta.json').read_text())
        self.assertEqual(stored['files'], ['except.txt'])
        self.assertNotIn('192.0.2.1', json.dumps(stored))
        self.assertEqual(len(notify.pending(self.root)), 1)

    def test_groups_desync_seats(self):
        for seat in range(3):
            self.send(note('desync', '20261024-010203-desync-%d-1' % seat, session='1a2b3c4d seed 00c0ffee'),
                      make_zip({'SYNC_H%d.LOG' % seat: 'x'}))
        sessions = list((self.root / 'desync' / 'sessions').iterdir())
        self.assertEqual(len(sessions), 1)
        self.assertEqual(len(list(sessions[0].iterdir())), 3)
        last = json.loads(notify.pending(self.root)[-1].read_text())
        self.assertEqual(last['seats'], 3)
        self.assertTrue(last['folder'].startswith('desync/sessions/'))

    def test_refusals(self):
        data = make_zip({'except.txt': 'x'})
        cases = [(415, dict(content_type='text/plain')), (413, dict(length=receiver.MAX_BODY_BYTES + 1)),
                 (411, dict(length=0))]
        for status, options in cases:
            with self.assertRaises(receiver.ReportError) as caught:
                self.send(note(), data, **options)
            self.assertEqual(caught.exception.status, status)

    def test_a_resent_report_is_stored_once(self):
        data = make_zip({'except.txt': 'x'})
        self.assertTrue(self.send(note(), data)[1])
        folder, new = self.send(note(), data)
        self.assertFalse(new)
        self.assertEqual(len(notify.pending(self.root)), 1)


class NotifyTests(unittest.TestCase):
    def test_summary_names_no_player(self):
        line = notify.summary({'kind': 'desync', 'id': 'x', 'commit': '6d60b4a9abcdef12', 'files': ['a', 'b'],
                               'bytes': 2048, 'folder': 'desync/sessions/ab/x', 'session': 's', 'seats': 2})
        self.assertIn('Out-of-sync report x', line)
        self.assertIn('Seat 2', line)
        self.assertIn('2 KB', line)


if __name__ == '__main__':
    unittest.main()
