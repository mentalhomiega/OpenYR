---
key: SW.FireIntoShroud
scope: superweapontype
summary: "Whether the weapon may be aimed at unexplored shroud."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the cursor shows [`NoCursor=`](/keys/nocursor/) over shroud, and a click there fires nothing. Fogged cells that the player has explored are allowed. The test applies to the local player's cursor only.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.FireIntoShroud=no
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
