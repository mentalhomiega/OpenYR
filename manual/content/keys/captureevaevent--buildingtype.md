---
key: CaptureEvaEvent
scope: buildingtype
label: Capture announcement
summary: The announcer line the capturing player hears when an engineer takes this tech structure.
see_also: [NeedsEngineer, Capturable]
when_omitted:
  kind: value
  value: none
---

```ini title="rulesmd.ini"
[CAHOSP] ; Hospital
NeedsEngineer=yes
Capturable=true
CaptureEvaEvent=EVA_HospitalCaptured
```

The value names a line in the announcer file, such as `EVA_HospitalCaptured`. The key matters only on a [`NeedsEngineer=yes`](/keys/needsengineer/) structure.

When a player takes such a structure from another house, the player who took it hears this line. A player who lost it hears `EVA_TechBuildingLost`. Neither hears the general building-captured line, and the radar does not flash. A line the announcer file does not list, or an empty value, plays nothing.

Nobody hears either line when the structure goes to a house with [`MultiplayPassive=yes`](/keys/multiplaypassive/).
