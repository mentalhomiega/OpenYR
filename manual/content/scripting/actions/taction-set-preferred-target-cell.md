---
type: action
id: TACTION_SET_PREFERRED_TARGET_CELL
title: "Set Preferred Target Cell..."
summary: "Aims the super weapons the computer fires for the trigger's house at a waypoint."
valid_values:
  - "A waypoint, in the last field."
caveats:
  - "The computer fires the paradrop, the spy plane and the psychic reveal at the waypoint as soon as the weapon is ready. It does not look for a target or require an enemy first."
  - "We also fire the nuclear missile and the lightning storm at the waypoint as soon as they are ready, but only while the house has an enemy."
  - "A lightning storm waits while another storm is running or about to start."
  - "We do not fire the genetic mutator or the psychic dominator while the cell is set. The other super weapons choose their own targets. A house a player controls aims its own weapons."
  - "The cell stays until Clear Preferred Target Cell. It is kept in a [save game](/formats/save-games/)."
  - "The action fails when the trigger has no house or the waypoint does not exist."
related:
  - type: action
    id: TACTION_CLEAR_PREFERRED_TARGET_CELL
  - type: action
    id: TACTION_PREFERRED_TARGET
  - type: system
    id: superweapons
---

```ini title="map file"
[Actions]
Aim=1,135,0,364,0,0,0,0,NA ; the house's missile goes to waypoint NA
```
