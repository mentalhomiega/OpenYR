---
key: MoveSound
summary: Sounds, one picked at random, played while an object of this type moves.
see_also: [DieSound, "system:sound-effects"]
when_omitted:
  kind: value
  value: "none"
---

When an object of this type starts moving, one entry picked at random from this list starts playing at its position. The sound is placed, so it fades and pans with where the object is on screen; see [Placed sounds](/systems/sound-effects/#placed-sounds).

The sound keeps playing while the object moves. Once the object has stopped for a few game frames, a looping sound is told to finish its current loop and stop, so a brief halt does not cut it off. The sound also stops when the object leaves the map, for example by entering a transport or a structure. The next time the object moves, a new entry is picked.

Names are matched as described in [Writing the list](/keys/voiceselect/#writing-the-list).

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
MoveSound=TankMove1 ; a sound ID registered in SOUND.INI
```
