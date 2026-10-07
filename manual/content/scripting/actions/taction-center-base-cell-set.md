---
type: action
id: TACTION_CENTER_BASE_CELL_SET
title: "Center Base Cell Set..."
summary: "Makes a waypoint the center of the trigger house's base."
valid_values:
  - "A waypoint, in the last field."
caveats:
  - "The center no longer follows the house's structures. Anything that works from the center of a base uses the waypoint, such as teams going home and the spot the computer's paradrops and spy planes aim for."
  - "The cell stays until Center Base Cell Clear. It is kept in a [save game](/formats/save-games/)."
  - "The action fails when the trigger has no house or the waypoint does not exist."
related:
  - type: action
    id: TACTION_CENTER_BASE_CELL_CLEAR
  - type: action
    id: TACTION_BASE_BUILDING
---

```ini title="map file"
[Actions]
Home=1,137,0,15,0,0,0,0,P ; the house's base is centered on waypoint P
```
