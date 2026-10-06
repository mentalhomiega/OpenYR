---
key: Trailer
scope: aircrafttype
label: Aircraft trail
see_also: ["SpawnDelay", "Image"]
when_omitted:
  kind: value
  value: none
---

The aircraft drops one copy of the animation at its position on every game frame that is a multiple of [`SpawnDelay`](/keys/spawndelay/), whether it is flying or on the ground. Each puff starts one frame after it is dropped and plays through as many times as the animation's [`LoopCount`](/keys/loopcount/) says, at least once.

The frames are counted on the game clock, so every trailing aircraft in a match drops its puffs on the same frames.

The setting is read from the art section the aircraft's [`Image`](/keys/image/) names, not from its rules section.

```ini title="art.ini"
[MYROCKETART]      ; the Image ID of an AircraftType
Trailer=SMOKEY2    ; an AnimType registered in [Animations]
SpawnDelay=2
```
