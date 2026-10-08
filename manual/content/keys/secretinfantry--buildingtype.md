---
key: SecretInfantry
scope: buildingtype
label: Infantry a lab offers
see_also: [SecretUnit, SecretBuilding, SecretLab, "system:production"]
when_omitted:
  kind: value
  value: "none"
---

Names the InfantryType that a [`SecretLab=yes`](/keys/secretlab/) structure of this type offers its owner. The lab then offers this type instead of drawing an item, and it ignores `SecretUnit=` and `SecretBuilding=`. Unlike the draw, the key works in campaign missions too.

```ini title="rules.ini"
[CASLAB] ; example BuildingType
SecretLab=yes
SecretInfantry=SNIPE ; an InfantryType registered in [InfantryTypes]
```

The key has no effect on a type without `SecretLab=yes`. [What a lab offers](/systems/production/#what-a-lab-offers) gives the order of the three keys.
