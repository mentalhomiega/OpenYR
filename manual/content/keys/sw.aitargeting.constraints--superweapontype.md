---
key: SW.AITargeting.Constraints
scope: superweapontype
summary: "Names conditions that must hold before the computer fires this superweapon."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A list of `enemy` (the house has an enemy), `offensive_cell_set` and `offensive_cell_clear` (the preferred target cell is set or clear), `defensive_cell_set` and `defensive_cell_clear` (the defensive cell is), `lightningstorm_inactive`, `dominator_inactive`, `attacked` (a structure of the house was attacked within the last minute) and `lowpower`. All listed conditions must hold.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AITargeting.Constraints=enemy,lowpower
```

See [Ai targeting modes](/systems/superweapons/#ai-targeting-modes).
