---
key: CanDetonateTimeBomb
summary: "Lets the player set off an Ivan bomb early by clicking the unit carrying it."
see_also: [IvanTimedDelay, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "no"
---

With `CanDetonateTimeBomb=yes`, the player [sets off a bomb at once](/systems/ivan-bombs/#setting-a-bomb-off-early) by clicking the vehicle or soldier carrying it while it is the only object selected, if the player's house planted the bomb.

```ini title="rulesmd.ini"
[CombatDamage]
CanDetonateTimeBomb=yes
```
