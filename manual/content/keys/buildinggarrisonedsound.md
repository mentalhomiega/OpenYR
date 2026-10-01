---
key: BuildingGarrisonedSound
summary: The sound played when the player's first soldier moves into a garrisonable structure.
see_also: [BuildingAbandonedSound, "system:garrisons"]
when_omitted:
  kind: value
  value: none
---

Plays at the soldier, only for the player who owns it, when it is the first occupant of a [garrisonable](/systems/garrisons/) structure.

```ini title="rulesmd.ini"
[AudioVisual]
BuildingGarrisonedSound=MyGarrison ; a sound ID registered in SOUNDMD.INI
```
