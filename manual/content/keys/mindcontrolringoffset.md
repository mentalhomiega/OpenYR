---
key: MindControlRingOffset
summary: "How high above the object a control ring is drawn."
see_also: [PermaControlledAnimationType, "system:superweapons"]
when_omitted:
  kind: value
  value: "140"
---

The ring a psychic dominator puts over this object, [`PermaControlledAnimationType`](/keys/permacontrolledanimationtype/), is drawn this many leptons above the object's center.

```ini title="rulesmd.ini"
[MYUNIT] ; example VehicleType
MindControlRingOffset=200
```

Negative values draw the ring lower.
