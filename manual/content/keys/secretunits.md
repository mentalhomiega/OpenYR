---
key: SecretUnits
summary: Lists the vehicle types the tech secret labs on a map can draw.
see_also: [SecretInfantry, SecretBuildings, SecretLab, "system:production"]
when_omitted:
  kind: value
  value: ""
---

Lists the UnitTypes that the tech secret labs on a skirmish or network map can draw when the game starts. The list follows [`SecretInfantry`](/keys/secretinfantry/#scope-global-rules) and precedes [`SecretBuildings`](/keys/secretbuildings/) in the combined list that [the draw](/systems/production/#the-draw) picks from.

```ini title="rulesmd.ini"
[General]
SecretUnits=TNKD,TTNK,DTRUCK ; UnitTypes registered in [VehicleTypes]
```

A campaign mission draws nothing, so the list has no effect there.
