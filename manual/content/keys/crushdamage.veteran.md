---
key: CrushDamage.Veteran
summary: "Crush damage for a veteran object of this type."
see_also: [CrushDamage]
when_omitted:
  kind: inherited
  note: The value of CrushDamage.
---

`CrushDamage.Veteran` is an Ares key. A vehicle that crushes a veteran object of this type takes this much damage, in place of the [`CrushDamage`](/keys/crushdamage/) value.

```ini title="rulesmd.ini"
[MyBloatedBug] ; example InfantryType
CrushDamage.Veteran=50
```
