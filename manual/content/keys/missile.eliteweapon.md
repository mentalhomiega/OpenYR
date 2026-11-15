---
key: Missile.EliteWeapon
summary: "The weapon for the blast of a custom missile launched by an elite launcher."
see_also: ["Missile.Weapon"]
when_omitted:
  kind: value
  value: "none"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). It works as [`Missile.Weapon=`](/keys/missile.weapon/) for an elite launcher. With none set, an elite launcher's missile uses `Missile.EliteDamage` and `Missile.EliteWarhead`.
