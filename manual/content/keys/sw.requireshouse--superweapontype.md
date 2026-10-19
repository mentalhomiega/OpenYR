---
key: SW.RequiresHouse
scope: superweapontype
summary: "Limits where this superweapon may be aimed by who owns the object on the cell."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "No limit"
---

A list of `owner`, `allies`, `team` (owner and allies), `enemies`, `all` and `none`. When the key lists any house, the cell must hold an object whose owner is in the relation to the firing house that the list names; an empty cell is refused.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.RequiresHouse=enemies
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
