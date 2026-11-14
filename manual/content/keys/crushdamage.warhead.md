---
key: CrushDamage.Warhead
summary: "The warhead that deals a crusher's crush damage."
see_also: [CrushDamage, C4Warhead]
when_omitted:
  kind: inherited
  note: The rules' C4Warhead.
---

`CrushDamage.Warhead` is an Ares key. It names the warhead through which a vehicle takes the [`CrushDamage`](/keys/crushdamage/) of an object of this type it crushed. The hit has no attacker, so no one gains experience from it.

```ini title="rulesmd.ini"
[MyBloatedBug] ; example InfantryType
CrushDamage.Warhead=MyAcidWH
```
