#!/usr/bin/env python3
"""Count the INI keys of a mod by origin: this engine, Ares, Phobos or unknown.

Usage: python mod_scan.py MOD_FOLDER [--stock FILE] [--list-unknown]

Reads rulesmd.ini, artmd.ini, aimd.ini, their "mo" counterparts and the files
they pull in through [#include]. A file that is not loose in the folder is
read, in memory only, from the unencrypted MIX archives in the folder.
Prints key names and counts only; no value and no file content is printed,
copied or stored. Origin lists are the name files in the data folder next to
this script.

--stock FILE lists the key names of the stock Yuri's Revenge rules files, one
per line. A key in that list that this engine does not read is reported as
origin "stock" instead of "unknown".
"""
import argparse
import re
import struct
import sys
import zlib
from collections import Counter
from pathlib import Path

DATA = Path(__file__).resolve().parent / "data"
ROOT_FILES = ("rulesmd.ini", "artmd.ini", "aimd.ini", "rulesmo.ini", "artmo.ini", "aimo.ini")
ORIGINS = ("engine", "stock", "ares", "phobos")
SKIP_SECTIONS = {"#include", "#include+"}


def load_names(path):
    """Return (exact lower-case names, compiled patterns) from a name file."""
    exact, patterns = set(), []
    if not path.is_file():
        return exact, patterns
    for line in path.read_text(encoding="utf-8").splitlines():
        name = line.strip()
        if not name or name.startswith("#"):
            continue
        if "<" in name:
            pieces = re.split(r"<[^>]*>", name.lower())
            regex = ".+".join(re.escape(piece) for piece in pieces)
            patterns.append(re.compile("^" + regex + "$"))
        else:
            exact.add(name.lower())
    return exact, patterns


def load_origins(data_dir=DATA, stock=None):
    """Load the name lists; stock is an optional file of stock key names."""
    origins = {origin: load_names(data_dir / (origin + ".txt")) for origin in ORIGINS}
    origins["stock"] = load_names(stock) if stock else (set(), [])
    return origins


def classify(key, origins):
    """Return the origin of a key name, or "unknown"."""
    low = key.lower()
    variants = [low, re.sub(r"\(\w+\)$", "", low), re.sub(r"\d+$", "", low)]
    for origin in ORIGINS:
        exact, patterns = origins[origin]
        for variant in variants:
            if variant in exact or any(p.match(variant) for p in patterns):
                return origin
    return "unknown"


def find_file(folder, name):
    """Find a file case-insensitively inside folder; return None if absent."""
    wanted = name.lower().replace("\\", "/")
    parts = wanted.split("/")
    current = folder
    for part in parts:
        match = None
        if current.is_dir():
            for child in current.iterdir():
                if child.name.lower() == part:
                    match = child
                    break
        if match is None:
            return None
        current = match
    return current if current.is_file() else None


def mix_file_id(name):
    """Westwood file ID: CRC-32 of the upper-case name, padded as MIX requires."""
    data = name.upper().encode("latin-1")
    length = len(data)
    if length & 3:
        base = length & ~3
        data += bytes([length - base])
        data += bytes([data[base]]) * (3 - (length & 3))
    return zlib.crc32(data) & 0xFFFFFFFF


def read_mix_index(path):
    """Return {file id: (offset, size)} for an unencrypted MIX, or None.

    Encrypted archives and files that are not MIX archives give None.
    """
    try:
        with open(path, "rb") as handle:
            head = handle.read(10)
            if len(head) < 6:
                return None
            lead, flags = struct.unpack_from("<HH", head)
            if lead == 0:
                if flags & 0x0002:
                    return None
                count, = struct.unpack_from("<H", head, 4)
                entries_at = 10
            else:
                count = lead
                entries_at = 6
            handle.seek(entries_at)
            raw = handle.read(12 * count)
            if len(raw) != 12 * count:
                return None
            body = entries_at + 12 * count
            index = {}
            for number in range(count):
                file_id, offset, size = struct.unpack_from("<Iii", raw, 12 * number)
                index[file_id] = (body + offset, size)
            return index
    except OSError:
        return None


class Source:
    """A mod folder: loose files first, then unencrypted MIX archives."""

    def __init__(self, folder):
        self.folder = folder
        self.mixes = []
        if folder.is_dir():
            for child in sorted(folder.iterdir()):
                if child.is_file() and child.suffix.lower() == ".mix":
                    index = read_mix_index(child)
                    if index is not None:
                        self.mixes.append((child, index))

    def lookup(self, name):
        """Return (identity, text) for a file name, or None if it is absent."""
        loose = find_file(self.folder, name)
        if loose is not None:
            return str(loose.resolve()), loose.read_text(encoding="latin-1")
        file_id = mix_file_id(name.replace("\\", "/").split("/")[-1])
        for path, index in self.mixes:
            if file_id in index:
                offset, size = index[file_id]
                with open(path, "rb") as handle:
                    handle.seek(offset)
                    data = handle.read(size)
                return "%s:%08x" % (path.resolve(), file_id), data.decode("latin-1")
        return None


def scan_file(source, name, seen, entries):
    """Collect (section, key) pairs from a file and its [#include] files."""
    found = source.lookup(name)
    if found is None:
        return False
    identity, text = found
    if identity in seen:
        return True
    seen.add(identity)
    section = None
    includes = []
    for raw in text.splitlines():
        line = raw.split(";", 1)[0].strip()
        if not line:
            continue
        head = re.match(r"^\[([^\]]*)\]", line)
        if head:
            section = head.group(1).strip()
            continue
        if "=" not in line or section is None:
            continue
        key, value = line.split("=", 1)
        key = key.strip()
        if section.lower() in SKIP_SECTIONS:
            includes.append(value.strip())
            continue
        if key:
            entries.append((section, key))
    for target in includes:
        scan_file(source, target, seen, entries)
    return True


def scan_folder(folder):
    source = Source(folder)
    entries, seen, found = [], set(), []
    for name in ROOT_FILES:
        if scan_file(source, name, seen, entries):
            found.append(name)
    return entries, found


def summarize(entries, origins):
    """Return per-origin counts of distinct keys and of occurrences."""
    distinct = {}
    occurrences = Counter()
    ignored = 0
    for _, key in entries:
        if key.isdigit():
            ignored += 1
            continue
        origin = classify(key, origins)
        distinct.setdefault(key.lower(), origin)
        occurrences[origin] += 1
    keys = Counter(distinct.values())
    return keys, occurrences, ignored, distinct


def coverage(keys):
    total = sum(keys.values())
    known = keys["engine"] + keys["ares"] + keys["phobos"] + keys["stock"]
    return known, total, (100.0 * known / total if total else 0.0)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("folder", type=Path)
    parser.add_argument("--stock", type=Path,
                        help="file of stock Yuri's Revenge key names, one per line")
    parser.add_argument("--list-unknown", action="store_true",
                        help="print the unknown key names (names only)")
    args = parser.parse_args(argv)
    if not args.folder.is_dir():
        print("not a folder: %s" % args.folder, file=sys.stderr)
        return 2
    entries, found = scan_folder(args.folder)
    if not found:
        print("no rules, art or ai INI file found in %s" % args.folder, file=sys.stderr)
        return 2
    if args.stock and not args.stock.is_file():
        print("not a file: %s" % args.stock, file=sys.stderr)
        return 2
    origins = load_origins(stock=args.stock)
    keys, occurrences, ignored, distinct = summarize(entries, origins)
    print("files read: " + ", ".join(found))
    print("%-10s %10s %12s" % ("origin", "distinct", "occurrences"))
    for origin in ORIGINS + ("unknown",):
        print("%-10s %10d %12d" % (origin, keys[origin], occurrences[origin]))
    known, total, pct = coverage(keys)
    print("covered: %d of %d distinct keys (%.1f%%)" % (known, total, pct))
    if total:
        print("read by this engine: %d of %d distinct keys (%.1f%%)"
              % (keys["engine"], total, 100.0 * keys["engine"] / total))
    print("numeric list keys ignored: %d" % ignored)
    if args.list_unknown:
        for name in sorted(k for k, v in distinct.items() if v == "unknown"):
            print("unknown: " + name)
    return 0


if __name__ == "__main__":
    sys.exit(main())
