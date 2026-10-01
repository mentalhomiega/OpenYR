---
key: DominatorFireAtPercentage
summary: "How far through its first animation the psychic dominator fires."
see_also: [DominatorFirstAnim, "system:superweapons"]
when_omitted:
  kind: value
  value: "50"
---

The blast fires once [`DominatorFirstAnim`](/keys/dominatorfirstanim/) has played this percentage of the frames in its image file.

```ini title="rulesmd.ini"
[General]
DominatorFireAtPercentage=20
```

At `0` or below, the blast fires two frames after the shot.
