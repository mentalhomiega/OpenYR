---
key: Rubble
summary: The shape a LeaveRubble structure leaves behind in place of frame 3 of its own image.
see_also: [LeaveRubble, IsRubble]
when_omitted:
  kind: value
  value: none
---

A [`LeaveRubble=yes`](/keys/leaverubble/) structure whose art entry names a shape here leaves that shape's first frame across its footprint when it is destroyed. The shape is looked up like the structure's other art, with the theater's extension. It casts no shadow. Without this key the ruins come from the fourth frame of the structure's own image, which must then have more than three frames before its shadow frames.

```ini title="artmd.ini"
[MYCIVBUILDING] ; example BuildingType
Rubble=MYRUBBLE ; a shape file, without its extension
```
