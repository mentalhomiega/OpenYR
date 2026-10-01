---
key: MuzzleFlash7
summary: The screen point that the eighth soldier in a garrison fires from.
see_also: [MuzzleFlash0, CanBeOccupied, MaxNumberOccupants, PrimaryFirePixelOffset, "system:garrisons"]
when_omitted:
  kind: value
  value: 0,0
---

`MuzzleFlash7` works like [`MuzzleFlash0`](/keys/muzzleflash0/) for the eighth soldier to move into the structure. It is read only when [`MaxNumberOccupants`](/keys/maxnumberoccupants/) is 8 or more.

```ini title="artmd.ini"
[MYOFFICES] ; the Image ID of a BuildingType
MuzzleFlash7=8,-30
```
