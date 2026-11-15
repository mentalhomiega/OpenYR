---
key: AttachedSystem
scope: bullettype
label: Smoke system trailing a projectile
summary: "A smoke particle system that follows each projectile of this type in flight."
see_also: [Trailer, "system:particle-systems"]
when_omitted:
  kind: value
  value: none
---

`AttachedSystem` on a projectile type is an Ares key. Each projectile of this type gets one particle system of the named type when it is launched, and the system moves with the projectile. When the projectile explodes or is removed, the system stops making particles and ends once the particles it made have died; it can also end sooner by itself, as [Ending a system](/systems/particle-systems/#ending-a-system) describes. Only a system with [`BehavesLike=Smoke`](/keys/behaveslike/#scope-particlesystemtype) is attached; a system of another kind is ignored.

```ini title="rulesmd.ini"
[MyProjectile] ; example BulletType
AttachedSystem=SmallGreySSys
```
