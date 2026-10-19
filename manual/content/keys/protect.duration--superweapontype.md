---
key: Protect.Duration
scope: superweapontype
summary: "Sets how many frames an iron curtain or force shield protects."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[CombatDamage] IronCurtainDuration; [General] ForceShieldDuration"
---

A `Type=IronCurtain` weapon protects each object for this many frames, and a `Type=ForceShield` weapon each structure. A negative value, or a missing key, uses the rules value.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Protect.Duration=600
```

See [Iron curtain](/systems/superweapons/#iron-curtain).
