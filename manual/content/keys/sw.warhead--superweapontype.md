---
key: SW.Warhead
scope: superweapontype
summary: "Names the warhead a generic warhead, missile strike, lightning bolt or dominator blast detonates."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A `Type=GenericWarhead` weapon detonates this warhead on the target cell and does nothing without it. A `Type=MultiMissile` weapon replaces its payload weapon's warhead with it. A `Type=LightningStorm` weapon uses it for every bolt in place of `LightningWarhead`, and a `Type=PsychicDominator` weapon uses it for the blast in place of `DominatorWarhead`. A `Type=GeneticConverter` weapon uses it in place of `MutateExplosionWarhead` or `MutateWarhead`. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Warhead=HE
```

See [Generic warhead](/systems/superweapons/#generic-warhead).
