#!/usr/bin/env python3
"""Count the INI keys of a mod by origin: this engine, Ares, Phobos or unknown.

Usage: python mod_scan.py MOD_FOLDER [--top N] [--list-unknown]

Reads rulesmd.ini, artmd.ini and the files they pull in through [#include].
Prints key names and counts only; no value and no file content is printed,
copied or stored. Origin lists are the name files in the data folder next to
this script.
"""
import argparse
import re
import sys
from collections import Counter
from pathlib import Path

DATA = Path(__file__).resolve().parent / "data"
ROOT_FILES = ("rulesmd.ini", "artmd.ini")
ORIGINS = ("engine", "ares", "phobos")
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


def load_origins(data_dir=DATA):
    return {origin: load_names(data_dir / (origin + ".txt")) for origin in ORIGINS}


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


def read_lines(path):
    return path.read_text(encoding="latin-1").splitlines()


def scan_file(path, folder, seen, entries):
    """Collect (section, key) pairs from path and its [#include] files."""
    resolved = path.resolve()
    if resolved in seen:
        return
    seen.add(resolved)
    section = None
    includes = []
    for raw in read_lines(path):
        line = raw.split(";", 1)[0].strip()
        if not line:
            continue
        head = re.match(r"^\[([^\]]*)\]", line)
        if head:
            section = head.group(1).strip()
            continue
        if "=" not in line or section is None:
            continue
        key, _ = line.split("=", 1)
        key = key.strip()
        if section.lower() in SKIP_SECTIONS:
            target = find_file(folder, line.split("=", 1)[1].strip())
            if target:
                includes.append(target)
            continue
        if key:
            entries.append((section, key))
    for target in includes:
        scan_file(target, folder, seen, entries)


def scan_folder(folder):
    entries, seen, found = [], set(), []
    for name in ROOT_FILES:
        path = find_file(folder, name)
        if path:
            found.append(name)
            scan_file(path, folder, seen, entries)
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
    known = keys["engine"] + keys["ares"] + keys["phobos"]
    return known, total, (100.0 * known / total if total else 0.0)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("folder", type=Path)
    parser.add_argument("--list-unknown", action="store_true",
                        help="print the unknown key names (names only)")
    args = parser.parse_args(argv)
    if not args.folder.is_dir():
        print("not a folder: %s" % args.folder, file=sys.stderr)
        return 2
    entries, found = scan_folder(args.folder)
    if not found:
        print("no rulesmd.ini or artmd.ini in %s" % args.folder, file=sys.stderr)
        return 2
    origins = load_origins()
    keys, occurrences, ignored, distinct = summarize(entries, origins)
    print("files read: " + ", ".join(found))
    print("%-10s %10s %12s" % ("origin", "distinct", "occurrences"))
    for origin in ORIGINS + ("unknown",):
        print("%-10s %10d %12d" % (origin, keys[origin], occurrences[origin]))
    known, total, pct = coverage(keys)
    print("covered: %d of %d distinct keys (%.1f%%)" % (known, total, pct))
    print("numeric list keys ignored: %d" % ignored)
    if args.list_unknown:
        for name in sorted(k for k, v in distinct.items() if v == "unknown"):
            print("unknown: " + name)
    return 0


if __name__ == "__main__":
    sys.exit(main())
