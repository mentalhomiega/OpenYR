---
key: EliteSBarrelThickness
summary: Parsed barrel thickness of an object's `EliteSecondary` slot. No firing point or effect uses it.
no_effect: true
see_also: [SBarrelThickness, EliteSecondary, "system:firing-geometry"]
when_omitted:
  kind: inherited
  note: The same section's SBarrelThickness= value.
---

`EliteSBarrelThickness=` is read into the elite secondary slot and stored. No firing point or effect reads it, so an elite object's second-slot shots stay where [`EliteSecondaryFireFLH`](/keys/elitesecondaryfireflh/) and [`TurretOffset`](/keys/turretoffset/) put them.
