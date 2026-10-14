---
key: PBarrelThickness
summary: Parsed lift of the first weapon's barrel pivot. No firing point or effect uses it.
no_effect: true
see_also: ["PrimaryFireFLH", "PBarrelLength", "ElitePBarrelThickness", "SBarrelThickness"]
when_omitted:
  kind: value
  value: "0"
---

The value is read into the first weapon slot and stored. No firing point or effect reads it, so the mounting that [`PrimaryFireFLH`](/keys/primaryfireflh/) sets stays where it is.

The elite primary slot's [`ElitePBarrelThickness`](/keys/elitepbarrelthickness/) defaults to this value. It has no effect either.
