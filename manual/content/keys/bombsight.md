---
key: BombSight
summary: "How near an Ivan bomb must be for this object to show it to the player, in cells."
see_also: [BombDisarm, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "0"
---

An object of this type that belongs to the player [shows the player](/systems/ivan-bombs/#seeing-and-disarming-bombs) every bomb less than this many cells away. With `0`, it shows none.

```ini title="rulesmd.ini"
[ENGINEER]
BombSight=4
```
