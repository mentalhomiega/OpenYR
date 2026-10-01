---
key: AIBasePlanningSide
summary: The side whose computer houses may put this type in their base plans.
see_also: ["system:ai-base-building", "BuildBarracks", "BuildRefinery", "BuildPower"]
when_omitted:
  kind: value
  value: "-1"
---

A computer house plans its base only with structures whose `AIBasePlanningSide` is `-1` or the index of the house's own side: `0` for the first side, `1` for the second and `2` for the third. With `-1`, every side may plan with it. This applies both to the structures it considers and to the one it picks for a role such as its barracks, refinery or power plant.

```ini title="rulesmd.ini"
[MyBarracks] ; example BuildingType
AIBasePlanningSide=1
```

Both also pass over a type whose [`RequiredHouses`](/keys/requiredhouses/) leaves the house out or whose [`ForbiddenHouses`](/keys/forbiddenhouses/) names it.
