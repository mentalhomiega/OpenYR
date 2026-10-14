---
key: FlyBy
summary: Keeps an aircraft in the game after it leaves the local area, until it leaves the playfield.
see_also: ["FlyBack", "Fighter", "AirportBound"]
when_omitted:
  kind: value
  value: "no"
---

`FlyBy=yes` keeps an aircraft in the game after it leaves the [`LocalSize`](/keys/localsize/) area. A loaner, or a team member on its way out, is then removed only once it leaves the [playfield](/keys/size/). Without the key, it is removed as soon as it leaves the local area, once it has entered the playable area.

```ini title="rules.ini"
[ORCA]
FlyBy=yes   ; stays in the game past the local area, until it leaves the playfield
```
