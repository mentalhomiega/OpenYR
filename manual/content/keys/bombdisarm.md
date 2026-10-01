---
key: BombDisarm
summary: "Makes a warhead remove the Ivan bomb from its target instead of damaging it."
see_also: [BombSight, IvanBomb, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "no"
---

A `BombDisarm=yes` warhead [removes the bomb](/systems/ivan-bombs/#seeing-and-disarming-bombs) from the object it hits and does no damage. A weapon with it fires only at an object carrying a bomb.

```ini title="rulesmd.ini"
[BombDisarm] ; example Warhead
BombDisarm=yes
```
