---
type: mission
id: TMISSION_CONVERT_TYPE
title: Convert type
summary: Turns every vehicle in the team into the type its Convert.Script names.
valid_values:
  - The line has no parameter. Write `0`.
caveats:
  - This is an Ares line, number 68.
  - Each vehicle whose type sets [`Convert.Script`](/keys/convert.script/) becomes that type, as [`Convert.Water`](/keys/convert.water/) does for a vehicle on water. Vehicles whose type sets none, infantry and aircraft stay as they are.
  - A vehicle that cannot take the type, because its [`Locomotor`](/keys/locomotor/) differs, keeps its own, and the other members still convert. The team takes the next line on the same turn.
related:
  - type: system
    id: ai-team-execution
  - type: key
    id: Convert.Script
---

```ini title="AI.INI, AIFS.INI, or map file"
[MyShiftScript] ; defined under [ScriptTypes]
Name=Convert the team
0=68,0 ; every vehicle with a Convert.Script type converts
```
