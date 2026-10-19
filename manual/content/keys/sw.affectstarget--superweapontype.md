---
key: SW.AffectsTarget
scope: superweapontype
summary: "Chooses which cells and objects an iron curtain covers."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "Everything"
---

The list takes the same names as [`SW.RequiresTarget=`](/keys/sw.requirestarget/). A `Type=IronCurtain` weapon protects only objects of the listed kinds, on cells of the listed ground. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AffectsTarget=units,buildings
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
