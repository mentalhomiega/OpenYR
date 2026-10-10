---
type: action
id: TACTION_RETINT_GREEN
title: "Retint Green..."
summary: "Sets the green part of the map's lighting tint."
valid_values:
  - "A number in the first field after the type: the tint in hundredths. `100` leaves green unchanged, as `Green=1.0` does under `[Lighting]`. A tint above `200` acts as `200`."
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
    id: TACTION_RETINT_BLUE
---

```ini title="map file"
[Actions]
Tint=1,143,0,150,0,0,0,0,A ; green at one and a half times
```
