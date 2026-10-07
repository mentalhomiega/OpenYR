---
type: action
id: TACTION_SET_TAB
title: "Set Tab to..."
summary: "Switches the sidebar to the tab the action names."
valid_values:
  - "A tab number from `0` to `3`: structures, defenses, infantry, and vehicles with aircraft."
caveats:
  - "A tab with nothing to build in it is not shown, and the action fails."
  - "The sidebar changes for the local player, whichever house owns the trigger."
related:
  - type: system
    id: sidebar
  - type: action
    id: TACTION_FLASH_CAMEO
---

```ini title="map file"
[Actions]
Infantry=1,114,0,2,0,0,0,0,A ; the infantry tab
```
