---
key: LeptonMindControlOffset
summary: "How high above this object a mind control line ends."
see_also: [MindControlAttackLineFrames, MindControlRingOffset, "system:mind-control"]
when_omitted:
  kind: value
  value: "70"
---

The line joining a [mind control](/systems/mind-control/#taking-an-object-over) firer to this object ends this many leptons above the object's position.

```ini title="rulesmd.ini"
[MYUNIT] ; example VehicleType
LeptonMindControlOffset=120
```
