---
key: UnitsGainSelfHeal
summary: How much this structure speeds the healing of all its owner's vehicles.
see_also: [InfantryGainSelfHeal, SelfHealUnitFrames, SelfHealUnitAmount, "system:repair"]
when_omitted:
  kind: value
  value: "0"
---

While this structure stands, every damaged vehicle its owner has gains [`SelfHealUnitAmount`](/keys/selfhealunitamount/) strength times this value every [`SelfHealUnitFrames`](/keys/selfhealunitframes/) frames. Several such structures add their values together. Aircraft gain nothing. [Healing from support structures](/systems/repair/#healing-from-support-structures) gives the details.

```ini title="rulesmd.ini"
[MYMACHINESHOP] ; example BuildingType
UnitsGainSelfHeal=1
```

A value of `0` or below adds nothing.
