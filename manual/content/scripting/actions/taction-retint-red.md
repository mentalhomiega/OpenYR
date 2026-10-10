---
type: action
id: TACTION_RETINT_RED
title: "Retint Red..."
summary: "Sets the red part of the map's lighting tint."
valid_values:
  - "A number in the first field after the type: the tint in hundredths. `100` leaves red unchanged, as `Red=1.0` does under `[Lighting]`. A tint above `200` acts as `200`."
caveats:
  - "Terrain and the objects on it are drawn in the new tint at once. The sidebar keeps its colors."
  - "The other two parts of the tint keep their values."
  - "The action succeeds whichever house owns the trigger."
related:
  - type: action
    id: TACTION_SET_AMBIENT_LIGHT
  - type: action
    id: TACTION_RETINT_GREEN
  - type: action
    id: TACTION_RETINT_BLUE
---

```ini title="map file"
[Actions]
Tint=1,142,0,150,0,0,0,0,A ; red at one and a half times
```
