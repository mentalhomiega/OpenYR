---
key: SecretUnit
summary: Names the vehicle type a tech secret lab offers.
see_also: [SecretInfantry, SecretBuilding, SecretLab, "system:production"]
when_omitted:
  kind: value
  value: "none"
---

Names the UnitType that a [`SecretLab=yes`](/keys/secretlab/) structure of this type offers its owner. The lab then offers this type instead of drawing an item, unless the type also sets [`SecretInfantry=`](/keys/secretinfantry/#scope-buildingtype), which wins. [`SecretBuilding=`](/keys/secretbuilding/) is ignored. Unlike the draw, the key works in campaign missions too.

```ini title="rules.ini"
[CASLAB] ; example BuildingType
SecretLab=yes
SecretUnit=DTRUCK ; a UnitType registered in [VehicleTypes]
```

The key has no effect on a type without `SecretLab=yes`. [What a lab offers](/systems/production/#what-a-lab-offers) gives the order of the three keys.
