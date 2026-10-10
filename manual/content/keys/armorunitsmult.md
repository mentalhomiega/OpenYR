---
key: ArmorUnitsMult
summary: The divisor on the damage a house of this country's vehicles take.
see_also: [ArmorInfantryMult, ArmorAircraftMult, ArmorBuildingsMult, Armor, Verses, "system:warheads"]
when_omitted:
  kind: value
  value: "1.0"
---

Damage to a vehicle owned by a house of this country is divided by this value, so a value above 1 makes such objects harder to destroy and one below 1 makes them easier. The division applies in every game type, once per hit, and the result is rounded down. [What the target loses](/systems/warheads/#what-the-target-loses) places it among the other reductions.

```ini title="rulesmd.ini"
[MYCOUNTRY] ; example country
ArmorUnitsMult=1.25 ; example: these objects take 20% less damage from each hit
```

Damage that bypasses armor, such as a planted C4 charge, is not divided, and healing is never reduced. A `0` here reduces every hit to 1 point before the warhead's `Verses` and the distance falloff are applied, so keep the value above `0`.
