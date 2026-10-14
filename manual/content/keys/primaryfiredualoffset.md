---
key: PrimaryFireDualOffset
summary: Whether a building's mounting adds its firing offset to PrimaryFirePixelOffset, instead of using the pixel offset alone.
see_also: ["PrimaryFirePixelOffset", "PrimaryFireFLH", "PBarrelLength", "PBarrelThickness"]
when_omitted:
  kind: value
  value: "no"
---

With `PrimaryFireDualOffset=yes`, the mounting of a building that sets [`PrimaryFirePixelOffset`](/keys/primaryfirepixeloffset/) is the point [`PrimaryFireFLH`](/keys/primaryfireflh/) gives, with the pixel offset added. Every part of a shot that starts at the mounting uses that point, including the projectile. The firing slot's own offset applies, and the offset turns with the gun.

Without it, the mounting is the pixel offset alone, measured from the point the building's artwork is drawn at, and the firing offsets are ignored.

```ini title="art.ini"
[GAPILL] ; the Image ID of a BuildingType
PrimaryFirePixelOffset=0,10
PrimaryFireFLH=90,0,40
PrimaryFireDualOffset=yes
```

The key has no effect on a building whose `PrimaryFirePixelOffset` is `65535,65535`.
