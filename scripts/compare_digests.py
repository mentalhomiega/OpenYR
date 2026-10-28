#!/usr/bin/env python3
"""Compares the picture or game-state digests of two debug logs and reports the first difference.

Run with `python -I scripts/compare_digests.py [--present|--hash] [--first] <a.log> <b.log>`. By
default it compares the DRAW_DIGEST lines of the drawdigest autotest step; `--present` compares the
PRESENT_DIGEST lines of the presentdigest step instead, and `--hash` the `AUTOTEST hash` lines of the
hash step (docs/AUTOTEST.md). Lines are paired by game frame and by their order within that frame,
so a line missing from one log does not shift the pairing of the others. `--first` keeps only the
first line of each game frame.

Exit codes: 0 when every line in both logs has the same hash (and layer); 3 when a hash or layer
differs, or a line appears in only one log; 4 when the logs share no line; 1 for a usage or read error.
"""

import re
import sys

PATTERNS = {
    "draw": re.compile(r"DRAW_DIGEST frame (-?\d+) draw (\d+) hash (\S+)()"),
    "present": re.compile(r"PRESENT_DIGEST frame (-?\d+) present (\d+) hash (\S+) layer (\S+)"),
    "hash line": re.compile(r"AUTOTEST hash frame (-?\d+) ()(\S+)()"),
}


def read_digests(path, pattern, first_only):
    """Returns {(frame, ordinal): (index, hash, layer)} for the digest lines in the log."""
    digests = {}
    per_frame = {}
    try:
        with open(path, "r", encoding="latin-1", errors="replace") as handle:
            for line in handle:
                match = pattern.search(line)
                if not match:
                    continue
                frame = int(match.group(1))
                ordinal = per_frame.get(frame, 0)
                per_frame[frame] = ordinal + 1
                if first_only and ordinal > 0:
                    continue
                index = int(match.group(2)) if match.group(2) else ordinal
                digests[(frame, ordinal)] = (index, match.group(3), match.group(4))
    except OSError as error:
        print(f"compare_digests: cannot read {path}: {error}", file=sys.stderr)
        sys.exit(1)
    return digests


def main(argv):
    args = argv[1:]
    kind = "draw"
    first_only = False
    while args and args[0] in ("--present", "--hash", "--first"):
        if args[0] in ("--present", "--hash"):
            kind = "present" if args[0] == "--present" else "hash line"
        else:
            first_only = True
        args = args[1:]
    if len(args) != 2:
        print("usage: python -I scripts/compare_digests.py [--present|--hash] [--first] <a.log> <b.log>", file=sys.stderr)
        return 1

    a = read_digests(args[0], PATTERNS[kind], first_only)
    b = read_digests(args[1], PATTERNS[kind], first_only)
    common = sorted(set(a) & set(b))
    only_a = sorted(set(a) - set(b))
    only_b = sorted(set(b) - set(a))
    differing = [key for key in common if a[key][1:] != b[key][1:]]

    print(f"{kind}s: a {len(a)}, b {len(b)}, in both {len(common)}, differing {len(differing)}, "
          f"only in a {len(only_a)}, only in b {len(only_b)}")

    if differing:
        frame, ordinal = differing[0]
        pa, pb = a[(frame, ordinal)], b[(frame, ordinal)]
        print(f"first difference: frame {frame}, order {ordinal} within the frame")
        print(f"  a: {kind} {pa[0]} hash {pa[1]} {pa[2]}".rstrip())
        print(f"  b: {kind} {pb[0]} hash {pb[1]} {pb[2]}".rstrip())
    if only_a:
        frame, ordinal = only_a[0]
        print(f"first {kind} only in a: frame {frame}, order {ordinal} within the frame")
    if only_b:
        frame, ordinal = only_b[0]
        print(f"first {kind} only in b: frame {frame}, order {ordinal} within the frame")

    if not common:
        print(f"no {kind} in both logs", file=sys.stderr)
        return 4
    if differing or only_a or only_b:
        return 3
    print("identical")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
