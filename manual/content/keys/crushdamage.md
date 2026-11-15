---
key: CrushDamage
summary: "Damage a vehicle takes when it crushes an object of this type, at every rank."
see_also: [CrushDamage.Rookie, CrushDamage.Veteran, CrushDamage.Elite, CrushDamage.Warhead, Crushable]
when_omitted:
  kind: value
  value: "0"
---

`CrushDamage` is an Ares key. A vehicle that crushes an object of this type by driving over it takes this much damage through [`CrushDamage.Warhead`](/keys/crushdamage.warhead/), so the warhead's `Verses` against the vehicle's armor apply. The damage follows the crush; the crushed object dies as before. A vehicle that crushes several such objects in one cell takes each one's damage in turn.

The key sets the value for all three ranks of the crushed object. [`CrushDamage.Rookie`](/keys/crushdamage.rookie/), [`CrushDamage.Veteran`](/keys/crushdamage.veteran/) and [`CrushDamage.Elite`](/keys/crushdamage.elite/) in the same section then override it for their rank, wherever they are written in it. Only crushing by a moving vehicle deals the damage; a unit that lands on an object or chronoshifts onto it takes none.

```ini title="rulesmd.ini"
[MyBloatedBug] ; example InfantryType
CrushDamage=50
CrushDamage.Elite=100
```
