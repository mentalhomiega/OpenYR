---
key: EliteSBarrelLength
summary: Parsed barrel length of an object's `EliteSecondary` slot. No firing point or effect uses it.
no_effect: true
see_also: [SBarrelLength, EliteSecondary, "system:firing-geometry"]
when_omitted:
  kind: inherited
  note: The same section's SBarrelLength= value.
---

`EliteSBarrelLength=` is read into the elite secondary slot and stored. No firing point or effect reads it, so an elite object's second-slot shots stay where [`EliteSecondaryFireFLH`](/keys/elitesecondaryfireflh/) and [`TurretOffset`](/keys/turretoffset/) put them.
