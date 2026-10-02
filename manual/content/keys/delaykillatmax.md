---
key: DelayKillAtMax
summary: "How much longer a CausesDelayKill fuse is at the edge of the blast."
see_also: [CausesDelayKill, DelayKillFrames]
when_omitted:
  kind: value
  value: "1.0"
---

A [`CausesDelayKill=yes`](/keys/causesdelaykill/) warhead's fuse is this many times [`DelayKillFrames`](/keys/delaykillframes/) for a structure at the edge of its `CellSpread`. The fuse grows in a straight line from the center, so with `DelayKillFrames=5` and `DelayKillAtMax=7.0` in a 4-cell blast, a structure 2 cells away waits 20 frames. With `1.0`, every structure waits the same time.

```ini title="rulesmd.ini"
[OilExplosionWH]
DelayKillAtMax=7.0
```
