---
key: CrashingSound
summary: "The sound played as a destroyed aircraft starts to fall."
see_also: [VoiceCrashing]
when_omitted:
  kind: value
  value: none
---

When an aircraft of this type is destroyed in the air, this sound plays where it is as it starts to fall. Objects that do not fly with the aircraft locomotor, such as jumpjet units, do not play it yet.

```ini title="rulesmd.ini"
[MYPLANE] ; example AircraftType
CrashingSound=MyPlaneDie
```
