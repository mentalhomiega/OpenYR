---
key: SecretBuilding
summary: Names the structure type a tech secret lab offers.
see_also: [SecretInfantry, SecretUnit, SecretLab, "system:production"]
when_omitted:
  kind: value
  value: "none"
---

Names the BuildingType that a [`SecretLab=yes`](/keys/secretlab/) structure of this type offers its owner. The lab then offers this type instead of drawing an item, unless the type also sets [`SecretInfantry=`](/keys/secretinfantry/#scope-buildingtype) or [`SecretUnit=`](/keys/secretunit/), which win in that order. Unlike the draw, the key works in campaign missions too.

```ini title="rules.ini"
[CASLAB] ; example BuildingType
SecretLab=yes
SecretBuilding=GTGCAN ; a BuildingType registered in [BuildingTypes]
```

The key has no effect on a type without `SecretLab=yes`. [What a lab offers](/systems/production/#what-a-lab-offers) gives the order of the three keys.
