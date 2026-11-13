---
key: CaptureEvaEvent
summary: The announcer line the capturing player hears when an engineer takes a structure of this type.
see_also: [NeedsEngineer, Capturable, "Message.Capture", LostEvaEvent, "system:capture"]
when_omitted:
  kind: value
  value: "none"
---

`CaptureEvaEvent` names a line of [EVAMD.INI](/formats/eva-ini/), such as `EVA_HospitalCaptured`. The player who takes a [`NeedsEngineer=yes`](/keys/needsengineer/) structure of this type from another house hears the line. A line the announcer file does not list, or no key, plays nothing. The key does nothing on a structure that does not need an engineer.

The capturing player hears neither the general building-captured line nor a radar event for such a structure. The player who lost it hears [`LostEvaEvent`](/keys/lostevaevent/). Nobody hears either line when the structure goes to a house with [`MultiplayPassive=yes`](/keys/multiplaypassive/).

```ini title="rulesmd.ini"
[CAHOSP] ; Hospital
NeedsEngineer=yes
Capturable=true
CaptureEvaEvent=EVA_HospitalCaptured
```
