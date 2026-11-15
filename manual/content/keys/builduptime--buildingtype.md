---
key: BuildupTime
scope: buildingtype
summary: "How long a structure of this type plays its construction animation, in game minutes."
see_also: [SellTime, Buildup]
when_omitted:
  kind: inherited
  note: The [General] BuildupTime.
---

`BuildupTime` on a structure type is an Ares key. A newly placed structure of this type plays its [buildup art](/keys/buildup/) over this many game minutes, in place of the [`[General]` value](/keys/builduptime/#scope-global-rules); a game minute is 900 frames. The structure finishes construction when the animation ends, so the value also sets how long it stays under construction.

The time is divided evenly across the animation's steps and rounded down to a whole frame per step. A value too small for that plays each step for one frame, so the animation takes longer than the value asks instead of being skipped. The type's value also replaces the five-second timing a [`Theater=yes`](/keys/theater/) structure gets at the start of a scenario.

Without [`SellTime`](/keys/selltime/), the sell animation uses this value too.

```ini title="rulesmd.ini"
[MYPLANT] ; example BuildingType
BuildupTime=.5  ; 450 frames, half a minute
```
