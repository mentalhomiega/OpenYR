---
key: IsRubble
summary: "Marks the overlay that the ruins of destroyed LeaveRubble structures are drawn on."
see_also: [LeaveRubble]
when_omitted:
  kind: value
  value: "no"
---

The first OverlayType with `IsRubble=yes` is placed on every cell of a destroyed [`LeaveRubble=yes`](/keys/leaverubble/) structure's footprint. An `IsRubble` overlay never draws its own image. The cell at the structure's top left corner draws the structure's ruins, and the other cells draw nothing. Set this on one OverlayType only; any others with it are not used for ruins but still draw nothing.

```ini title="rulesmd.ini"
[MYRUBBLE] ; example OverlayType
IsRubble=yes
LegalTarget=no
```
