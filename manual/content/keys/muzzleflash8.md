---
key: MuzzleFlash8
summary: The screen point that the ninth soldier in a garrison fires from.
see_also: [MuzzleFlash0, CanBeOccupied, MaxNumberOccupants, PrimaryFirePixelOffset, "system:garrisons"]
when_omitted:
  kind: value
  value: 0,0
---

`MuzzleFlash8` works like [`MuzzleFlash0`](/keys/muzzleflash0/) for the ninth soldier to move into the structure. It is read only when [`MaxNumberOccupants`](/keys/maxnumberoccupants/) is 9 or more.

```ini title="artmd.ini"
[MYOFFICES] ; the Image ID of a BuildingType
MuzzleFlash8=8,-30
```
