---
key: SBarrelThickness
summary: Parsed lift of the second weapon's barrel pivot. No firing point or effect uses it.
no_effect: true
see_also: ["SecondaryFireFLH", "SBarrelLength", "PBarrelThickness"]
when_omitted:
  kind: value
  value: "0"
---

The value is read into the second weapon slot and stored. No firing point or effect reads it, so the mounting that [`SecondaryFireFLH`](/keys/secondaryfireflh/) sets stays where it is.

The elite secondary slot's [`EliteSBarrelThickness`](/keys/elitesbarrelthickness/) defaults to this value. It has no effect either.
