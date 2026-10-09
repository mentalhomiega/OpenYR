---
key: InfantryGainSelfHeal
summary: How much this structure speeds the healing of all its owner's infantry.
see_also: [UnitsGainSelfHeal, SelfHealInfantryFrames, SelfHealInfantryAmount, "system:repair"]
when_omitted:
  kind: value
  value: "0"
---

While this structure stands, every damaged infantryman its owner has, and every damaged vehicle with [`Organic=yes`](/keys/organic/), gains [`SelfHealInfantryAmount`](/keys/selfhealinfantryamount/) strength times this value every [`SelfHealInfantryFrames`](/keys/selfhealinfantryframes/) frames. Several such structures add their values together. [Healing from support structures](/systems/repair/#healing-from-support-structures) gives the details.

```ini title="rulesmd.ini"
[MYHOSPITAL] ; example BuildingType
InfantryGainSelfHeal=1
```

A value of `0` or below adds nothing.
