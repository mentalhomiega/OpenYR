---
key: DieSound
summary: Sounds, one picked at random, played where an object of this type is destroyed.
see_also: [VoiceDie, "system:destruction-and-debris"]
when_omitted:
  kind: value
  value: "none"
---

When damage destroys an object, one entry picked at random from this list plays at the object's position, right after the [`VoiceDie`](/keys/voicedie/) sound if the type has one. The sound is placed, so it fades and pans with where the object is on screen, and every player in view hears it whoever owns the object; see [Placed sounds](/systems/sound-effects/#placed-sounds).

Names are matched as described in [Writing the list](/keys/voiceselect/#writing-the-list).

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
DieSound=TankExplode1,TankExplode2 ; sound IDs registered in SOUND.INI
```
