---
key: SecretBuildings
summary: Lists the structure types the tech secret labs on a map can draw.
see_also: [SecretInfantry, SecretUnits, SecretLab, "system:production"]
when_omitted:
  kind: value
  value: ""
---

Lists the BuildingTypes that the tech secret labs on a skirmish or network map can draw when the game starts. The list is the last part of the combined list that [the draw](/systems/production/#the-draw) picks from, after [`SecretInfantry`](/keys/secretinfantry/#scope-global-rules) and [`SecretUnits`](/keys/secretunits/).

```ini title="rulesmd.ini"
[General]
SecretBuildings=GTGCAN ; BuildingTypes registered in [BuildingTypes]
```

A campaign mission draws nothing, so the list has no effect there.
