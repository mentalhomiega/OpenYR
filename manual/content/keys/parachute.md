---
key: Parachute
summary: The canopy drawn above a paradropped passenger on the way down.
see_also: [BombParachute, ChuteSound, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

```ini title="rulesmd.ini"
[General]
Parachute=MYCHUTE ; an AnimType registered in [Animations]
```

Every passenger dropped from an aircraft falls with this animation attached above it as a canopy until it lands. Passengers are dropped two ways:

- A [paradrop](/systems/superweapons/#paradrops) plane drops its paratroopers over its target.
- Any other aircraft carrying passengers drops one of them each time it would fire its weapon, at the aircraft's position.

If the ground below will not take the passenger, it goes back into the hold and no canopy is created.

The key is read from `[General]` and then from `[AudioVisual]`, so an `[AudioVisual]` entry wins. Every paradropped passenger uses this canopy; [`BombParachute`](/keys/bombparachute/) is never used.

The passenger falls at the slow parachute rate only while the animation lasts. Give it a [`LoopCount=`](/keys/loopcount/) long enough to play until landing; if it ends early, or the key is unset, the passenger falls faster.
