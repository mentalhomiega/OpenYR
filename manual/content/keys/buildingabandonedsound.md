---
key: BuildingAbandonedSound
summary: The sound played when the last of the player's soldiers leaves a garrisoned structure.
see_also: [BuildingGarrisonedSound, "system:garrisons"]
when_omitted:
  kind: value
  value: none
---

Plays for the player whose garrison has just emptied, as the [garrisonable](/systems/garrisons/) structure passes back to the civilians. It is not tied to a place on the map, so it is heard at the same volume wherever the player is looking.

```ini title="rulesmd.ini"
[AudioVisual]
BuildingAbandonedSound=MyAbandon ; a sound ID registered in SOUNDMD.INI
```
