---
key: SecretLab
summary: Makes this structure a tech secret lab, which lets its owner build one item without meeting that item's prerequisites.
see_also: [SecretInfantry, SecretUnit, SecretBuilding, SecretUnits, SecretBuildings, "system:production"]
when_omitted:
  kind: value
  value: "no"
---

A `SecretLab=yes` structure offers its owner one infantry, vehicle or structure type to build whatever that type's prerequisites, tech level and country limits say. The owner keeps the item only while a lab that offers it is on the map under their control. The item is chosen when a skirmish or network game starts, or named by [`SecretInfantry=`](/keys/secretinfantry/#scope-buildingtype), [`SecretUnit=`](/keys/secretunit/) or [`SecretBuilding=`](/keys/secretbuilding/) on the lab's type. [Tech secret labs](/systems/production/#tech-secret-labs) gives the rules in full.

```ini title="rules.ini"
[CASLAB] ; example BuildingType
SecretLab=yes
```
