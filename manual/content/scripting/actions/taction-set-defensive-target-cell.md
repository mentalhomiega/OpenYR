---
type: action
id: TACTION_SET_DEFENSIVE_TARGET_CELL
title: "Set Defensive Target Cell..."
summary: "Has a computer house raise its force shield over a waypoint."
valid_values:
  - "A waypoint, in the last field."
caveats:
  - "The trigger's house wants its force shield over the waypoint for [`AISuperDefenseFrames`](/keys/aisuperdefenseframes/) frames from the moment the action runs. The computer fires a charged force shield at the cell in its next AI pass."
  - "Only a computer house fires its force shield by itself, so the action does nothing visible for a house a person plays. It also does nothing for a computer house with no charged force shield."
  - "A weapon fired near the house's base can replace the cell with its construction yard."
  - "The action fails when the trigger has no house or the waypoint does not exist."
related:
  - type: action
    id: TACTION_CLEAR_DEFENSIVE_TARGET_CELL
  - type: action
    id: TACTION_SET_PREFERRED_TARGET_CELL
  - type: system
    id: superweapons
---

```ini title="map file"
[Actions]
Shield=1,140,0,0,0,0,0,0,C ; the trigger's house wants its force shield over waypoint 2
```
