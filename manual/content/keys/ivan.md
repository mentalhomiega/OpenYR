---
key: Ivan
summary: "Makes the player's attack orders with this soldier show the Ivan bomb cursor."
see_also: [Bombable, IvanBomb, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "no"
---

When the player points a soldier of this type at a target, the cursor [shows whether it can be bombed](/systems/ivan-bombs/#ordering-a-bomb), and a target that cannot takes no order.

```ini title="rulesmd.ini"
[MYIVAN] ; example InfantryType
Ivan=yes
```
