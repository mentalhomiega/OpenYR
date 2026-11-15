---
key: SW.AffectsTarget
scope: superweapontype
summary: "Chooses which cells and objects an iron curtain, chronosphere, genetic mutator or psychic dominator affects."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "Everything; infantry and units for a chronosphere or psychic dominator"
---

The list takes the same names as [`SW.RequiresTarget=`](/keys/sw.requirestarget/). A `Type=IronCurtain` weapon protects only objects of the listed kinds, on cells of the listed ground. A `Type=ChronoSphere` or `Type=PsychicDominator` weapon affects only infantry and vehicles unless the key is set, and `all` or `none` lifts every restriction. A `Type=GeneticConverter` weapon without `MutateExplosion=yes` reads only `land` and `water` from the list. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AffectsTarget=units,buildings
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
