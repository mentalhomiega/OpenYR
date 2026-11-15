---
key: Missile.Warhead
summary: "The warhead of a custom missile launched by a launcher that is not elite."
see_also: ["Missile.EliteWarhead", "Missile.Weapon"]
when_omitted:
  kind: value
  value: "none"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). The missile explodes with this warhead and [`Missile.Damage=`](/keys/missile.damage/). Without a warhead it explodes without damage.
