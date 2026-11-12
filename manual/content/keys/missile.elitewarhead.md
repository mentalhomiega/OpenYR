---
key: Missile.EliteWarhead
summary: "The warhead of a custom missile launched by an elite launcher."
see_also: ["Missile.Warhead", "Missile.EliteWeapon"]
when_omitted:
  kind: value
  value: "none"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). The missile explodes with this warhead and [`Missile.EliteDamage=`](/keys/missile.elitedamage/). It does not fall back to `Missile.Warhead`.
