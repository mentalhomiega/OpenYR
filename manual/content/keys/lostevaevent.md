---
key: LostEvaEvent
summary: The EVA line the player hears on losing a structure of this type that needs an engineer to a capture.
see_also: [NeedsEngineer, "Message.Lost", CaptureEvaEvent, "system:capture"]
when_omitted:
  kind: computed
  note: EVA_TechBuildingLost when the type sets Message.Lost, otherwise nothing.
---

`LostEvaEvent` names the line of [EVAMD.INI](/formats/eva-ini/) that the player hears when an enemy takes a [`NeedsEngineer=yes`](/keys/needsengineer/) structure of this type from the player. `none` announces nothing.

A type that sets [`Message.Lost`](/keys/message.lost/) and no `LostEvaEvent` announces `EVA_TechBuildingLost`. A type that sets neither announces only the ordinary capture line.

```ini title="rulesmd.ini"
[CAOILD] ; example BuildingType
NeedsEngineer=yes
LostEvaEvent=EVA_TechBuildingLost
```
