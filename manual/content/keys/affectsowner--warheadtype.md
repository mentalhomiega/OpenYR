---
key: AffectsOwner
scope: warheadtype
summary: "Whether a warhead affects the objects of the house that fired it."
see_also: [AffectsAllies, AffectsEnemies]
when_omitted:
  kind: inherited
  note: The value of AffectsAllies.
---

`AffectsOwner=no` makes the warhead pass over the objects of the house that fired it, and `yes` lets it hit them. The key decides only for the firer's own house. An object of an allied house follows [`AffectsAllies=`](/keys/affectsallies/), and any other object follows [`AffectsEnemies=`](/keys/affectsenemies/). `AffectsOwner=yes` with `AffectsAllies=no` therefore hits only the firer's own objects. The warhead spares an object only when it knows its firer, as `AffectsAllies=no` already does.

```ini title="rulesmd.ini"
[MyHealWH] ; example Warhead
AffectsAllies=yes
AffectsOwner=no  ; heals allies but not the firer's own objects
```
