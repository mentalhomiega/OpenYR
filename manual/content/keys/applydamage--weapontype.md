---
key: ApplyDamage
scope: weapontype
summary: "Whether the weapon's shots carry its Damage."
see_also: [Damage, IsSonic, UseFireParticles]
when_omitted:
  kind: computed
  note: "no for a weapon with IsSonic=yes or UseFireParticles=yes, otherwise yes"
---

With `ApplyDamage=no`, the shots of the weapon carry no damage, and the target's estimated health does not drop for them either. With `yes`, they carry the weapon's `Damage=` even for a weapon with `IsSonic=yes` or `UseFireParticles=yes`, which otherwise damage only through their wave or particles.

```ini title="rulesmd.ini"
[MySonicGun] ; example WeaponType
IsSonic=yes
Damage=20
ApplyDamage=yes
```
