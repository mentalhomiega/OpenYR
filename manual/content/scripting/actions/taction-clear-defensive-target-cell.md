---
type: action
id: TACTION_CLEAR_DEFENSIVE_TARGET_CELL
title: "Clear Defensive Target Cell"
summary: "Drops the cell a computer house wants its force shield over."
caveats:
  - "The house stops wanting its force shield anywhere until another weapon fired near its base, or a Set Defensive Target Cell action, sets a cell."
  - "The action fails when the trigger has no house. It succeeds when no cell was set."
related:
  - type: action
    id: TACTION_SET_DEFENSIVE_TARGET_CELL
  - type: system
    id: superweapons
---
