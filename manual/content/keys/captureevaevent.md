---
key: CaptureEvaEvent
summary: The EVA line the player hears on capturing a structure of this type that needs an engineer.
see_also: [NeedsEngineer, "Message.Capture", LostEvaEvent, "system:capture"]
when_omitted:
  kind: value
  value: "none"
---

`CaptureEvaEvent` names the line of [EVAMD.INI](/formats/eva-ini/) that the player hears when an engineer takes a [`NeedsEngineer=yes`](/keys/needsengineer/) structure of this type. The line follows the ordinary capture announcement. A value of `none`, or no key, adds nothing.

The line is spoken only to the player who captures. [`LostEvaEvent`](/keys/lostevaevent/) covers the player who loses the structure.

```ini title="rulesmd.ini"
[CAOILD] ; example BuildingType
NeedsEngineer=yes
CaptureEvaEvent=EVA_TechBuildingCaptured
```
