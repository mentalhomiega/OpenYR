---
key: SW.NegBuildings
scope: superweapontype
summary: "Withdraws this superweapon while the house owns any of the listed structures."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

The weapon is not held while the house owns a structure of any listed type, and returns when the last one is gone. The test runs whenever the house updates [which superweapons it holds](/systems/superweapons/#becoming-available).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.NegBuildings=GAWEAP
```

See [Availability rules](/systems/superweapons/#availability-rules).
