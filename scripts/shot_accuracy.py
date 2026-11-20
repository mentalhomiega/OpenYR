"""Hit-rate table from the SHOTTRACE lines of a shot-accuracy.txt run.

Usage: python scripts/shot_accuracy.py <test script> <debug log or filtered output>

Each "; block <name> x y" line of the script names a block by the cell of its first firer. A shot belongs to the block whose
firer column (or the Howitzer column three cells west) and seven rows, give or take a cell for the muzzle, hold the cell
it was fired from.
"""
import re
import sys
from collections import defaultdict

SHOT = re.compile(r"SHOTTRACE frame \d+ (\S+) (\S+) by (\S+) from (-?\d+),(-?\d+) at (\S+) (-?\d+),(-?\d+) (\w+) "
                  r"off (-?\d+),(-?\d+),(-?\d+) cells (-?\d+),(-?\d+) agl (-?\d+) tagl (-?\d+)")


def blocks_of(script):
    blocks = []
    for line in open(script, encoding="latin1"):
        match = re.match(r";\s*block (\S+) (\d+) (\d+)", line)
        if match:
            blocks.append((match.group(1), int(match.group(2)), int(match.group(3))))
    return blocks


def block_at(blocks, x, y):
    for name, x0, y0 in blocks:
        if (abs(x - x0) <= 1 or abs(x - x0 + 3) <= 1) and y0 - 1 <= y <= y0 + 7:
            return name
    return None


def main():
    blocks = blocks_of(sys.argv[1])
    counts = defaultdict(lambda: [0, 0])
    misses = defaultdict(list)
    for line in open(sys.argv[2], encoding="latin1"):
        match = SHOT.search(line)
        if not match:
            continue
        firer, fx, fy, result = match.group(3), int(match.group(4)), int(match.group(5)), match.group(9)
        block = block_at(blocks, fx, fy)
        if block is None or result == "ground":
            continue
        key = (block, firer)
        counts[key][0] += 1
        if result == "hit":
            counts[key][1] += 1
        else:
            misses[key].append("%s,%s,%s" % (match.group(10), match.group(11), match.group(12)))
    firers = sorted({firer for _, firer in counts})
    print("%-11s" % "block" + "".join("%11s" % firer for firer in firers))
    for name, _, _ in blocks:
        cells = []
        for firer in firers:
            shots, hits = counts.get((name, firer), (0, 0))
            cells.append("%11s" % ("%d/%d" % (hits, shots) if shots else "-"))
        print("%-11s" % name + "".join(cells))
    if "-v" in sys.argv:
        for key in sorted(misses):
            print("miss %s %s: %s" % (key[0], key[1], " ".join(misses[key][:8])))


if __name__ == "__main__":
    main()
