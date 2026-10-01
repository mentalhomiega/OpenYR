---
key: MuzzleFlash6
summary: The screen point that the seventh soldier in a garrison fires from.
see_also: [MuzzleFlash0, CanBeOccupied, MaxNumberOccupants, PrimaryFirePixelOffset, "system:garrisons"]
when_omitted:
  kind: value
  value: 0,0
---

`MuzzleFlash6` works like [`MuzzleFlash0`](/keys/muzzleflash0/) for the seventh soldier to move into the structure. It is read only when [`MaxNumberOccupants`](/keys/maxnumberoccupants/) is 7 or more.

```ini title="artmd.ini"
[MYOFFICES] ; the Image ID of a BuildingType
MuzzleFlash6=8,-30
```
