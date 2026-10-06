---
key: FlyBack
summary: Keeps an aircraft from being removed from the game when it flies off the map, so that it can return.
see_also: ["FlyBy", "Fighter", "AirportBound"]
when_omitted:
  kind: value
  value: "no"
---

`FlyBack=yes` exempts an aircraft from the rule that removes a loaner, or a team member on its way out, once it has left the map. The aircraft stays in the game while it is off the map and can fly back onto it.

```ini title="rules.ini"
[BEAG]
FlyBack=yes   ; is not removed after flying off the map
```
