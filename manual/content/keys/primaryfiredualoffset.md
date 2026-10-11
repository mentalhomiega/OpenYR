---
key: PrimaryFireDualOffset
summary: Whether a building's muzzle follows its firing offset plus PrimaryFirePixelOffset, instead of the pixel offset alone.
see_also: ["PrimaryFirePixelOffset", "PrimaryFireFLH", "PBarrelLength", "PBarrelThickness"]
when_omitted:
  kind: value
  value: "no"
---

With `PrimaryFireDualOffset=yes`, the [muzzle](/systems/firing-geometry/#the-mounting-and-the-muzzle) of a building that sets [`PrimaryFirePixelOffset`](/keys/primaryfirepixeloffset/) is the point [`PrimaryFireFLH`](/keys/primaryfireflh/) gives, with the pixel offset added. The muzzle is where the firing animation, laser beam, sonic wave, electric bolt and weapon particle effects appear. The firing slot's own offset, barrel length and barrel thickness apply, and the offset turns with the gun.

Without it, the muzzle is the pixel offset alone, measured from the point the building's artwork is drawn at, and the firing offsets are ignored. The mounting, where the projectile is created, is always the pixel offset alone.

```ini title="art.ini"
[GAPILL] ; the Image ID of a BuildingType
PrimaryFirePixelOffset=0,10
PrimaryFireFLH=90,0,40
PrimaryFireDualOffset=yes
```

The key has no effect on a building whose `PrimaryFirePixelOffset` is `65535,65535`.
