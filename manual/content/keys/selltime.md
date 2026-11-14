---
key: SellTime
summary: "How long a structure of this type takes to play its sell animation, in game minutes."
see_also: [BuildupTime, Buildup]
when_omitted:
  kind: inherited
  note: The construction animation's timing from BuildupTime.
---

`SellTime` is an Ares key. A structure of this type that is sold, or packs up into a vehicle, plays its [buildup art](/keys/buildup/) backwards over this many game minutes; a game minute is 900 frames. The structure leaves the map when the animation ends, so the value also sets how long the sale takes. Without the key the animation runs at the timing [`BuildupTime`](/keys/builduptime/) gives it.

The time is divided evenly across the animation's steps and rounded down to a whole frame per step. A value too small for that plays each step for one frame instead of skipping the animation.

```ini title="rulesmd.ini"
[MYMINE] ; example BuildingType
SellTime=.5  ; 450 frames, half a minute
```
