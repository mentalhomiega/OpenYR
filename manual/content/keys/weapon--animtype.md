---
key: Weapon
scope: animtype
summary: "The weapon whose projectile detonates where the animation is each time its Damage applies."
see_also: [Damage, "Damage.Delay", Warhead]
when_omitted:
  kind: value
  value: "none, so the animation deals its Damage as stock"
---

Each time the animation's `Damage` applies, which `Damage.Delay=` spaces out, the weapon's projectile detonates at the animation, with the weapon's damage and warhead and the effects of its projectile. The animation's own `Damage` value only decides when. A house owns the blast when the animation has an owner.

```ini title="artmd.ini"
[NAIRON_G] ; example AnimationType
Damage=1
Damage.Delay=2
Weapon=IronDeviceFlash
```
