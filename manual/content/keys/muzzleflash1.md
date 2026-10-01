---
key: MuzzleFlash1
summary: The screen point that the second soldier in a garrison fires from.
see_also: [MuzzleFlash0, CanBeOccupied, MaxNumberOccupants, PrimaryFirePixelOffset, "system:garrisons"]
when_omitted:
  kind: value
  value: 0,0
---

`MuzzleFlash1` works like [`MuzzleFlash0`](/keys/muzzleflash0/) for the second soldier to move into the structure. It is read only when [`MaxNumberOccupants`](/keys/maxnumberoccupants/) is 2 or more.

```ini title="artmd.ini"
[MYOFFICES] ; the Image ID of a BuildingType
MuzzleFlash1=8,-30
```
