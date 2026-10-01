---
key: MindControlRingOffset
summary: "How high above the object a control ring is drawn."
see_also: [PermaControlledAnimationType, ControlledAnimationType, "system:superweapons", "system:mind-control"]
when_omitted:
  kind: value
  value: "140"
---

The ring over this object while the psychic dominator or a [mind control](/systems/mind-control/) weapon holds it, [`PermaControlledAnimationType`](/keys/permacontrolledanimationtype/) or [`ControlledAnimationType`](/keys/controlledanimationtype/), is drawn this many leptons above the object's center. A structure's mind control ring uses its art `Height` instead.

```ini title="rulesmd.ini"
[MYUNIT] ; example VehicleType
MindControlRingOffset=200
```

Negative values draw the ring lower.
