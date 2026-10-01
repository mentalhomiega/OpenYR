---
key: MaxNumberOccupants
summary: How many soldiers a garrisonable structure holds.
see_also: [CanBeOccupied, ShowOccupantPips, MuzzleFlash0, "system:garrisons"]
when_omitted:
  kind: value
  value: "0"
---

A [`CanBeOccupied=yes`](/keys/canbeoccupied/) structure takes soldiers until this many are inside. At `0` or below, no soldier can move in.

```ini title="rulesmd.ini"
[MYOFFICES] ; example BuildingType
CanBeOccupied=yes
MaxNumberOccupants=6
```

The value also sets the length of the structure's occupant pip row, and which of the art entry's muzzle points are read: [`MuzzleFlash0`](/keys/muzzleflash0/) through `MuzzleFlash9` are read only for slots below this number. Occupants past the tenth fire from `MuzzleFlash9`.
