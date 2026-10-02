---
key: LeaveRubble
summary: "Leaves the structure's ruins on the ground where it stood when it is destroyed."
see_also: [IsRubble, CrateBeneath]
when_omitted:
  kind: value
  value: "no"
---

With `LeaveRubble=yes`, a destroyed structure of this type leaves its ruins behind once it has left the map. Every cell of its footprint gets the first OverlayType that sets [`IsRubble=yes`](/keys/isrubble/), and the ruins are drawn across the footprint from the fourth frame of the structure's own image, with the matching shadow frame. The ruins are drawn in the theater palette, so that frame must be painted for it.

Nothing is left when no OverlayType sets `IsRubble=yes`, or when the structure's image has fewer than four frames before its shadow frames. Whether units can cross the ruins depends on that OverlayType's `Land`.

```ini title="rulesmd.ini"
[MYCIVBUILDING] ; example BuildingType
LeaveRubble=yes
```
