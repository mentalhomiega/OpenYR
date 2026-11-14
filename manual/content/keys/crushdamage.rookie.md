---
key: CrushDamage.Rookie
summary: "Crush damage for a rookie object of this type."
see_also: [CrushDamage]
when_omitted:
  kind: inherited
  note: The value of CrushDamage.
---

`CrushDamage.Rookie` is an Ares key. A vehicle that crushes a rookie object of this type takes this much damage, in place of the [`CrushDamage`](/keys/crushdamage/) value.

```ini title="rulesmd.ini"
[MyBloatedBug] ; example InfantryType
CrushDamage.Rookie=25
```
