---
key: MuzzleFlash0
summary: The screen point that the first soldier in a garrison fires from.
see_also: [CanBeOccupied, MaxNumberOccupants, PrimaryFirePixelOffset, "system:garrisons"]
when_omitted:
  kind: value
  value: 0,0
---

Two whole numbers, `X,Y`, in screen pixels from the point the structure's artwork is drawn at. A positive `X` moves right and a positive `Y` moves down. The soldier who moved into a [`CanBeOccupied=yes`](/keys/canbeoccupied/) structure first fires from `MuzzleFlash0`, the second from `MuzzleFlash1`, and so on up to `MuzzleFlash9`. Occupants past the tenth fire from `MuzzleFlash9`.

```ini title="artmd.ini"
[MYOFFICES] ; the Image ID of a BuildingType
MuzzleFlash0=-20,-40
MuzzleFlash1=12,-46
```

The point is both where the shot is created and where its fire animation appears. Like [`PrimaryFirePixelOffset`](/keys/primaryfirepixeloffset/), the offset moves the starting point across the map at the structure's height rather than raising it. While the structure has occupants, these points replace `PrimaryFirePixelOffset` and the structure's other firing offsets.

`MuzzleFlashN` is read only when [`MaxNumberOccupants`](/keys/maxnumberoccupants/) is greater than `N` at the time the structure's rules section is read. A point for a slot the structure does not have is ignored.
