---
key: LostEvaEvent
summary: The EVA line the player hears on losing a structure of this type that needs an engineer to a capture.
see_also: [NeedsEngineer, "Message.Lost", CaptureEvaEvent, "system:capture"]
when_omitted:
  kind: value
  value: "EVA_TechBuildingLost"
---

`LostEvaEvent` names the line of [EVAMD.INI](/formats/eva-ini/) that the player hears when an enemy takes a [`NeedsEngineer=yes`](/keys/needsengineer/) structure of this type from the player. A name that the announcer file does not list, including `none`, leaves the default line, so the player always hears one.

```ini title="rulesmd.ini"
[CAOILD] ; example BuildingType
NeedsEngineer=yes
LostEvaEvent=EVA_TechBuildingLost
```
