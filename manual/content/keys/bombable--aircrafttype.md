---
key: Bombable
scope: aircrafttype
label: Can be bombed on the player's order
see_also: [Ivan, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "yes"
---

With `Bombable=no`, the player cannot [order an `Ivan=yes` soldier](/systems/ivan-bombs/#ordering-a-bomb) to bomb an object of this type. A soldier that targets it on its own still can.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Bombable=no
```
