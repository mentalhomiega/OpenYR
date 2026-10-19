---
key: IsDetachedRailgun
summary: "Makes a weapon fire the railgun beam of IsRailgun without waiting for the previous beam to fade."
see_also: [IsRailgun, AttachedParticleSystem, ROF]
when_omitted:
  kind: value
  value: "no"
---

An `IsDetachedRailgun=yes` weapon fires the same beam as an [`IsRailgun=yes`](/keys/israilgun/) weapon: it damages the objects along the line from the mounting to the target as the weapon fires and draws its [`AttachedParticleSystem`](/keys/attachedparticlesystem/) along it. The firer does not remember the beam, so the next shot waits for [`ROF`](/keys/rof/#scope-weapontype) alone and neither of the object's weapons is held up by a beam that is still fading. Every shot leaves its own particle system, so a short `ROF` puts many on the map at once.

Do not set both `IsDetachedRailgun=yes` and `IsRailgun=yes` on one weapon. Give the weapon an `AttachedParticleSystem=`, as the game crashes the first time a railgun weapon without one fires.

```ini title="rulesmd.ini"
[MyRailgun] ; example WeaponType
IsDetachedRailgun=yes
AttachedParticleSystem=LargeRailgunSys
AmbientDamage=200
Damage=0
```
