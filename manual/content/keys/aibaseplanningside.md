---
key: AIBasePlanningSide
summary: The side whose computer houses may put this type in their base plans.
see_also: ["system:ai-base-building", "BuildBarracks", "BuildRefinery", "BuildPower"]
when_omitted:
  kind: value
  value: "-1"
---

When a computer house picks a structure for a role in its base plan, such as its barracks, refinery or power plant, it takes the first type in that role's list that it may own. A type with `AIBasePlanningSide` set is passed over unless the value is the index of the house's own side: `0` for the first side, `1` for the second and `2` for the third. With `-1`, every side may plan with it.

```ini title="rulesmd.ini"
[MyBarracks] ; example BuildingType
AIBasePlanningSide=1
```

The same pick also passes over a type whose [`RequiredHouses`](/keys/requiredhouses/) leaves the house out or whose [`ForbiddenHouses`](/keys/forbiddenhouses/) names it.
