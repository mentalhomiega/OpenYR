---
key: Assaulter
summary: Lets a soldier that cannot occupy clear a garrison of another house.
see_also: [Occupier, AssaultAnim, CanBeOccupied, "system:garrisons"]
when_omitted:
  kind: value
  value: "no"
---

An `Assaulter=yes` soldier with `Occupier=no` moves into a [`CanBeOccupied=yes`](/keys/canbeoccupied/) structure that has occupants and is not allied with its house. It kills every occupant, then steps away. [Clearing](/systems/garrisons/#clearing) gives the conditions and the result.

```ini title="rulesmd.ini"
[MYASSAULTER] ; example InfantryType
Assaulter=yes
```

A soldier with `Occupier=yes` joins the garrison as an occupant, whatever `Assaulter` is set to. No stock soldier sets `Assaulter=yes`.
