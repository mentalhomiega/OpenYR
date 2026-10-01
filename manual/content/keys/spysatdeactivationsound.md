---
key: SpySatDeactivationSound
summary: The sound the local player hears when their spy satellite stops showing them the map.
see_also: [SpySat, SpySatActivationSound, "system:map-visibility"]
when_omitted:
  kind: value
  value: none
---

Plays once, for the local player only, when the player's last working [`SpySat=yes`](/keys/spysat/) structure stops working and the map goes dark again.

```ini title="rulesmd.ini"
[AudioVisual]
SpySatDeactivationSound=MySatelliteOffline ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored and the sound set earlier stays.
