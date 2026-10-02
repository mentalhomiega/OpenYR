---
key: VoiceCrashing
summary: "The voice played as one of the player's destroyed aircraft starts to fall."
see_also: [CrashingSound]
when_omitted:
  kind: value
  value: none
---

Like [`CrashingSound`](/keys/crashingsound/), but played only when the falling aircraft belongs to the player.

```ini title="rulesmd.ini"
[MYPLANE] ; example AircraftType
VoiceCrashing=MyPilotScream
```
