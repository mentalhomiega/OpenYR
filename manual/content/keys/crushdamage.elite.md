---
key: CrushDamage.Elite
summary: "Crush damage for an elite object of this type."
see_also: [CrushDamage]
when_omitted:
  kind: inherited
  note: The value of CrushDamage.
---

`CrushDamage.Elite` is an Ares key. A vehicle that crushes an elite object of this type takes this much damage, in place of the [`CrushDamage`](/keys/crushdamage/) value.

```ini title="rulesmd.ini"
[MyBloatedBug] ; example InfantryType
CrushDamage.Elite=100
```
