---
key: AreaFire
scope: weapontype
label: 'Strikes the firer''s own cell'
see_also: [Deployer, DeployFire, DeployFireWeapon]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, every shot of the weapon lands on the firer's own cell, whatever it was aimed at, so the warhead's spread hits everything around the firer. Yuri's psychic wave and the Desolator's radiation work this way.

```ini title="rulesmd.ini"
[MyPulse] ; example Weapon
AreaFire=yes
FireOnce=yes
```

The key also changes how a [`Deployer=yes`](/keys/deployer/#scope-infantrytype), [`DeployFire=yes`](/keys/deployfire/#scope-infantrytype) soldier deploys when the weapon is in its [`DeployFireWeapon`](/keys/deployfireweapon/#scope-infantrytype) slot:

- The Deploy command makes the soldier fire the weapon at its own cell instead of deploying.
- The Desolator, whose type is named `DESO`, deploys as usual instead. While deployed with nothing to attack, it keeps firing the weapon at its own cell.
