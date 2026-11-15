---
key: Missile.Weapon
summary: "A weapon whose bullet makes the blast of a custom missile in place of its damage and warhead."
see_also: ["Missile.EliteWeapon", "Missile.Damage", "Missile.Warhead"]
when_omitted:
  kind: value
  value: "none"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). When the missile explodes at its nose, a bullet of the weapon's `Projectile` detonates there with the weapon's `Damage` and `Warhead`, and `Missile.Damage` and `Missile.Warhead` are not used. A launcher that is elite uses [`Missile.EliteWeapon=`](/keys/missile.eliteweapon/), and does not fall back to this weapon. Name the weapon in `[WeaponTypes]`.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.Weapon=MyMissileBlast
```
