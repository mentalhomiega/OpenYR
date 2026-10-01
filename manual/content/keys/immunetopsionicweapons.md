---
key: ImmuneToPsionicWeapons
summary: "Spares objects of this type from PsychicDamage warheads."
see_also: [PsychicDamage, ImmuneToPsionics]
when_omitted:
  kind: value
  value: "no"
---

An object of this type [takes no damage](/systems/warheads/#what-the-target-loses) from a [`PsychicDamage=yes`](/keys/psychicdamage/) warhead. Being taken over by mind control is a separate matter, which [`ImmuneToPsionics`](/keys/immunetopsionics/) controls.

```ini title="rulesmd.ini"
[MYPSYCHIC] ; example InfantryType
ImmuneToPsionicWeapons=yes
```
