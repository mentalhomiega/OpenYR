---
key: MuzzleFlash9
summary: The screen point that the tenth soldier in a garrison fires from.
see_also: [MuzzleFlash0, CanBeOccupied, MaxNumberOccupants, PrimaryFirePixelOffset, "system:garrisons"]
when_omitted:
  kind: value
  value: 0,0
---

`MuzzleFlash9` works like [`MuzzleFlash0`](/keys/muzzleflash0/) for the tenth soldier to move into the structure. It is read only when [`MaxNumberOccupants`](/keys/maxnumberoccupants/) is 10 or more. Every occupant past the tenth also fires from this point.

```ini title="artmd.ini"
[MYOFFICES] ; the Image ID of a BuildingType
MuzzleFlash9=8,-30
```
