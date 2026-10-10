---
type: action
id: TACTION_RETINT_BLUE
title: "Retint Blue..."
summary: "Sets the blue part of the map's lighting tint."
valid_values:
  - "A number in the first field after the type: the tint in hundredths. `100` leaves blue unchanged, as `Blue=1.0` does under `[Lighting]`. A tint above `200` acts as `200`."
caveats:
  - "Terrain and the objects on it are drawn in the new tint at once. The sidebar keeps its colors."
  - "The other two parts of the tint keep their values."
  - "The action succeeds whichever house owns the trigger."
related:
  - type: action
    id: TACTION_SET_AMBIENT_LIGHT
  - type: action
    id: TACTION_RETINT_RED
  - type: action
    id: TACTION_RETINT_GREEN
---

```ini title="map file"
[Actions]
Tint=1,144,0,150,0,0,0,0,A ; blue at one and a half times
```
