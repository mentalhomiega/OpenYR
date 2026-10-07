---
type: action
id: TACTION_TELEPORT_ALL_TO
title: "Teleport All to..."
summary: "Moves every vehicle and soldier of the trigger's house to a waypoint at once."
valid_values:
  - "A waypoint, in the last field."
caveats:
  - "The units fill the cells around the waypoint, nearest first, one cell each. A cell a unit cannot enter is skipped for that unit."
  - "A unit that can enter no cell around the waypoint, such as a ship with only land there, stays where it is. The other units still move."
  - "Passengers, aircraft and objects that are off the map stay where they are."
  - "No effect or sound plays. Each unit drops the move it was making and keeps its team and selection."
  - "The action fails when the trigger has no house or the waypoint does not exist. Otherwise it succeeds, even when no unit moved."
related:
  - type: action
    id: TACTION_CHRONO_SCREEN_EFFECT
  - type: action
    id: TACTION_RESHROUD_AT
---

```ini title="map file"
[Actions]
Move=1,128,0,68,0,0,0,0,BQ ; the house's units gather at waypoint BQ
```
