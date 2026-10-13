---
key: FlyBack
summary: Keeps an aircraft in the game after it leaves the local area, so that it can fly back onto it.
see_also: ["FlyBy", "Fighter", "AirportBound"]
when_omitted:
  kind: value
  value: "no"
---

`FlyBack=yes` keeps an aircraft in the game after it leaves the [`LocalSize`](/keys/localsize/) area, so that it can fly back onto it. A loaner, or a team member on its way out, is then removed only once it leaves the [playfield](/keys/size/). Without the key, it is removed as soon as it leaves the local area.

```ini title="rules.ini"
[BEAG]
FlyBack=yes   ; stays in the game past the local area, until it leaves the playfield
```
