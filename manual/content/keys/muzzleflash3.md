---
key: MuzzleFlash3
summary: The screen point that the fourth soldier in a garrison fires from.
see_also: [MuzzleFlash0, CanBeOccupied, MaxNumberOccupants, PrimaryFirePixelOffset, "system:garrisons"]
when_omitted:
  kind: value
  value: 0,0
---

`MuzzleFlash3` works like [`MuzzleFlash0`](/keys/muzzleflash0/) for the fourth soldier to move into the structure. It is read only when [`MaxNumberOccupants`](/keys/maxnumberoccupants/) is 4 or more.

```ini title="artmd.ini"
[MYOFFICES] ; the Image ID of a BuildingType
MuzzleFlash3=8,-30
```
