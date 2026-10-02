---
key: CausesDelayKill
summary: "Makes a warhead set a fuse on structures it hits instead of damaging them."
see_also: [DelayKillFrames, DelayKillAtMax, EligibleForDelayKill]
when_omitted:
  kind: value
  value: "no"
---

A `CausesDelayKill=yes` warhead does no damage to an [`EligibleForDelayKill=yes`](/keys/eligiblefordelaykill/) structure. Instead it drops the structure to 1 strength and destroys it after a delay, as if by a C4 charge. Healing does not start a fuse. Other objects take the warhead's damage as usual.

The delay is [`DelayKillFrames`](/keys/delaykillframes/) for a structure at the center of the blast, and grows in a straight line to [`DelayKillAtMax`](/keys/delaykillatmax/) times that at the edge of the warhead's `CellSpread`. A later blast shortens a fuse that is already burning, but never lengthens it. When a fuse runs out, the structure is destroyed with [`C4Warhead`](/keys/c4warhead/), so a row of barrels with a `CausesDelayKill` death weapon goes off one after another.

```ini title="rulesmd.ini"
[OilExplosionWH]
CellSpread=4
CausesDelayKill=yes
DelayKillFrames=5
DelayKillAtMax=7.0
```
