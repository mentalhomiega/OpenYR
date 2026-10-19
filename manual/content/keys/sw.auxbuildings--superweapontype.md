---
key: SW.AuxBuildings
scope: superweapontype
summary: "Holds this superweapon only while the house owns one of the listed structures."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

The weapon is held only while the house owns at least one structure of a listed type, in addition to the structure that grants it. An empty value or `none` lifts the condition. The test runs whenever the house updates [which superweapons it holds](/systems/superweapons/#becoming-available).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AuxBuildings=GAPILL,GAWEAP
```

See [Availability rules](/systems/superweapons/#availability-rules).
