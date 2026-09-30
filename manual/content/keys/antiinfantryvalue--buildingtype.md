---
key: AntiInfantryValue
scope: buildingtype
label: Anti-infantry defense value
summary: How strong the computer player rates this structure as a defense against infantry.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: "0"
---

The computer player uses this number when it plans its base defenses: a structure with a value above `0` is a candidate infantry defense, and a higher value makes it count for more where the computer weighs the defenses it has against the threat it expects. [Defense values](/systems/ai-base-building/#defense-values) describes how the value is used.

```ini title="rulesmd.ini"
[MyTower] ; example BuildingType
AntiInfantryValue=40
```

The value is not derived from the structure's weapon, so a defense that sets none of the three values is never built as a defense.
