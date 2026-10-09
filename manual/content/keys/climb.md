---
key: Climb
summary: The leptons of altitude a jumpjet unit gains or loses each game frame.
see_also: [CruiseHeight, WobbleDeviation]
when_omitted:
  kind: value
  value: "5"
---

```ini title="rules.ini"
[JumpjetControls]
Climb=10 ; reaches a 400-lepton cruise height in 40 frames
```

`Climb` sets how far a jumpjet rises or sinks in one frame to reach the height it wants. The distance is in leptons, 256 to a cell. At the default `5`, a jumpjet takes 80 frames, a little over five seconds at 15 frames a second, to climb to the default [`CruiseHeight`](/keys/cruiseheight/) of `400`.

Use a whole number. A jumpjet's height is kept in whole leptons, so a climb drops the fraction and a descent rounds it up. `Climb=5.5` climbs 5 leptons a frame and descends 6. A value below `1` never climbs at all.

A descent stops at ground level. When less than one step is left before a jumpjet reaches the height it wants, it moves the rest of the way in that frame, so it settles on that height without overshooting it.

`Climb` also sets how quickly a jumpjet lifts over terrain and structures. While moving, a jumpjet raises the height it wants to clear the cell ahead. It stops while it is less than half its flight level above the ground or structures under it, until it has climbed, unless it is on the cell it is flying to. A small value therefore makes a jumpjet stop and climb slowly at each obstacle.

This is the default for every type; a type's [`JumpjetClimb`](/keys/jumpjetclimb/) replaces it for that type.
