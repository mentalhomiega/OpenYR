---
key: SpySatActivationSound
summary: The sound the local player hears when a spy satellite starts showing them the map.
see_also: [SpySat, SpySatDeactivationSound, "system:map-visibility"]
when_omitted:
  kind: value
  value: none
---

Plays once, for the local player only, when the player's first working [`SpySat=yes`](/keys/spysat/) structure lifts the shroud.

```ini title="rulesmd.ini"
[AudioVisual]
SpySatActivationSound=MySatelliteOnline ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored and the sound set earlier stays.
