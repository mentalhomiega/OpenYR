---
key: PsychicDamage
summary: "Marks a warhead as psychic, so ImmuneToPsionicWeapons types take no damage from it."
see_also: [ImmuneToPsionicWeapons, "system:warheads"]
when_omitted:
  kind: value
  value: "no"
---

A `PsychicDamage=yes` warhead [does nothing](/systems/warheads/#what-the-target-loses) to a vehicle, infantryman, aircraft or structure whose type is [`ImmuneToPsionicWeapons=yes`](/keys/immunetopsionicweapons/).

```ini title="rulesmd.ini"
[MyPsiWave] ; example Warhead
PsychicDamage=yes
```
