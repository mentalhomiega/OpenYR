---
key: SecretInfantry
scope: global-rules
label: Infantry labs can draw
see_also: [SecretUnits, SecretBuildings, SecretLab, "system:production"]
when_omitted:
  kind: value
  value: ""
---

Lists the InfantryTypes that the tech secret labs on a skirmish or network map can draw when the game starts. The list is the first part of the combined list that [the draw](/systems/production/#the-draw) picks from, ahead of [`SecretUnits`](/keys/secretunits/) and [`SecretBuildings`](/keys/secretbuildings/).

```ini title="rulesmd.ini"
[General]
SecretInfantry=SNIPE,TERROR,DESO,YURI ; InfantryTypes registered in [InfantryTypes]
```

A campaign mission draws nothing, so the list has no effect there.
