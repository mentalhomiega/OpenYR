---
key: SuperWeapons
scope: buildingtype
label: Structure grants
see_also: [SuperWeapon, SuperWeapon2, AuxBuilding, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

A comma-separated list of superweapons that a standing structure of this type grants, in addition to [`SuperWeapon=`](/keys/superweapon/) and [`SuperWeapon2=`](/keys/superweapon2/). The house [holds each weapon](/systems/superweapons/#from-a-structure-or-a-plug) while it owns at least one such structure, subject to the same [`AuxBuilding=`](/keys/auxbuilding/) test and to the [availability rules](/systems/superweapons/#availability-rules). A name that matches no listed section is skipped. `SuperWeapon=` and `SuperWeapon2=` come first: they decide which weapon's announcement plays when the structure is placed, followed by this list in order, and a structure with no `SuperWeapon=` plays its charge animations for the first weapon of this list it grants.

A plug grants the weapons in its own `SuperWeapons=` as it does the weapon in `SuperWeapon=`.

```ini title="rules.ini"
[GATECH]
SuperWeapon=ParaDropSpecial
SuperWeapons=SpyPlaneSpecial,GenericStrikeSpecial
```
