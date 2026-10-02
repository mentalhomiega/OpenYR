---
key: DelayKillFrames
summary: "Frames before a CausesDelayKill warhead destroys a structure at the center of its blast."
see_also: [CausesDelayKill, DelayKillAtMax]
when_omitted:
  kind: value
  value: "0"
---

A [`CausesDelayKill=yes`](/keys/causesdelaykill/) warhead destroys an eligible structure at the center of its blast after this many frames. Farther structures wait longer, up to [`DelayKillAtMax`](/keys/delaykillatmax/) times this value at the edge of the blast.

```ini title="rulesmd.ini"
[OilExplosionWH]
DelayKillFrames=5
```
