---
key: AISuperDefenseDistance
scope: global-rules
label: Computer superweapon defense distance
summary: "How close to a computer base a weapon must land for the computer to raise its force shield."
see_also: ["system:superweapons", AIDefendAgainst, AISuperDefenseProbability, AISuperDefenseFrames]
when_omitted:
  kind: value
  value: "10"
---

A computer house considers raising its force shield only when the target of a weapon with [`AIDefendAgainst=yes`](/keys/aidefendagainst/) lies no farther than this from the center of its base. The value is in cells and may have a fraction; the engine keeps it in leptons. When the key is omitted the distance is 10 leptons, a twenty-fifth of a cell, so rules files set it.

```ini title="rulesmd.ini"
[General]
AISuperDefenseDistance=12 ; within twelve cells of the base
```

The distance is measured in a straight line, height included, from the center of the base to the center of the target cell.
