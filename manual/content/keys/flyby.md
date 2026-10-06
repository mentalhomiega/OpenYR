---
key: FlyBy
summary: Keeps an aircraft from being removed from the game when it flies off the map.
see_also: ["FlyBack", "Fighter", "AirportBound"]
when_omitted:
  kind: value
  value: "no"
---

`FlyBy=yes` exempts an aircraft from the rule that removes a loaner, or a team member on its way out, once it has left the map. The aircraft stays in the game while it is off the map.

```ini title="rules.ini"
[ORCA]
FlyBy=yes   ; is not removed after flying off the map
```
