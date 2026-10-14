---
key: ElitePBarrelThickness
summary: Parsed barrel thickness of an object's `ElitePrimary` slot. No firing point or effect uses it.
no_effect: true
see_also: [PBarrelThickness, ElitePrimary, "system:firing-geometry"]
when_omitted:
  kind: inherited
  note: The same section's PBarrelThickness= value.
---

`ElitePBarrelThickness=` is read into the elite primary slot and stored. No firing point or effect reads it, so an elite object's shots stay where [`ElitePrimaryFireFLH`](/keys/eliteprimaryfireflh/) and [`TurretOffset`](/keys/turretoffset/) put them.
