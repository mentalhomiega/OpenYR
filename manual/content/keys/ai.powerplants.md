---
key: AI.PowerPlants
summary: The structures a computer house of the country builds when it is short of power.
see_also: [RegularPowerPlant, AdvancedPowerPlant, PowerTurbine, BuildPower, "system:ai-base-building"]
when_omitted:
  kind: context-dependent
  note: "The country's side decides, through its regular and advanced power plants."
---

A list of BuildingTypes. When a computer house of the country is short of power, it inserts one power plant ahead of the structure it was about to build. With this key set, it inserts the last listed type whose prerequisites its buildings already satisfy, and the first listed type when none does. List the plain plant first and the upgraded one after it. At least one entry must always satisfy its prerequisites, or the house inserts a plant it cannot build.

The first listed type also takes the part of the side's regular power plant for the [turbine upgrade](/keys/powerturbine/). Types listed here count as power plants when a house takes over another house's base.

The country that counts is the one the house acts as.

```ini title="rulesmd.ini"
[Americans] ; example country
AI.PowerPlants=GAPOWR,GAPOWRUP
```
