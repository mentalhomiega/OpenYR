---
key: SW.Damage
scope: superweapontype
summary: "Sets the damage a generic warhead, missile strike, lightning bolt or dominator blast deals."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "-1"
---

A `Type=GenericWarhead` weapon detonates its warhead for this damage; a value of 0 or less does nothing. A `Type=MultiMissile` weapon replaces its payload weapon's damage with it when the value is 0 or more. A `Type=LightningStorm` weapon deals it with every bolt in place of `LightningDamage`, and a `Type=PsychicDominator` weapon deals it with the blast in place of `DominatorDamage`; with the dominator, 0 deals no damage. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Damage=400
```

See [Generic warhead](/systems/superweapons/#generic-warhead).
