---
key: RelativeDamage
scope: warheadtype
summary: "Makes the warhead hurt by a share of the victim's strength."
see_also: ["RelativeDamage.Buildings", "RelativeDamage.Aircraft", "RelativeDamage.Infantry", "RelativeDamage.Vehicles"]
when_omitted:
  kind: value
  value: "no, so the warhead hurts by the damage its shot carries"
---

With `RelativeDamage=yes`, each hit on a soldier, vehicle, aircraft or structure takes a share of the victim's strength, in place of the damage the shot carries. The percentage comes from the key for the victim's kind: [`RelativeDamage.Buildings=`](/keys/relativedamage.buildings/), [`RelativeDamage.Aircraft=`](/keys/relativedamage.aircraft/), [`RelativeDamage.Infantry=`](/keys/relativedamage.infantry/) or [`RelativeDamage.Vehicles=`](/keys/relativedamage.vehicles/). A vehicle with `Organic=yes` counts as infantry, and one with `ConsideredAircraft=yes` as an aircraft.

A kind with no percentage takes no damage. The share is then scaled as any damage is: by [`Verses`](/keys/verses/), the distance from the blast, the country's armor multiplier and the victim's veterancy. The sign of the shot's own `Damage=` still decides: a healing weapon heals by the share. A forced hit, such as a crush or a captured-structure charge, is not changed. Terrain is not covered, and takes the damage the shot carries.

```ini title="rulesmd.ini"
[MadWH] ; example WarheadType
Verses=100%,100%,100%,100%,100%,100%,100%,100%,100%,100%,100%
RelativeDamage=yes
RelativeDamage.Vehicles=50
RelativeDamage.Infantry=-50
```
