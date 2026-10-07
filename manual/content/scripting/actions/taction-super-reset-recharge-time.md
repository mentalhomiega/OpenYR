---
type: action
id: TACTION_SUPER_RESET_RECHARGE_TIME
title: "Superweapon Reset Recharge Time..."
summary: "Puts one of the trigger house's super weapons back to the charge time its type gives."
valid_values:
  - "A super weapon by its place in the `[SuperWeaponTypes]` list of the rules, counted from `0`, in the field after the type."
caveats:
  - "A weapon that is charging keeps the time it has left. The type's time applies from the next charge."
  - "A number off the list leaves the action doing nothing, and it fails."
related:
  - type: action
    id: TACTION_SUPER_SET_RECHARGE_TIME
  - type: system
    id: superweapons
---
