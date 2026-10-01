---
key: Bombable
summary: "Lets the player order an Ivan soldier to bomb an object of this type."
see_also: [Ivan, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "yes"
---

With `Bombable=no`, the player cannot [order an `Ivan=yes` soldier](/systems/ivan-bombs/#ordering-a-bomb) to bomb an object of this type. A soldier that targets it on its own still can.

```ini title="rulesmd.ini"
[MYWALL] ; example type
Bombable=no
```
