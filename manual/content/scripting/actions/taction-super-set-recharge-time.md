---
type: action
id: TACTION_SUPER_SET_RECHARGE_TIME
title: "Superweapon Set Recharge Time..."
summary: "Gives one of the trigger house's super weapons a charge time of its own."
valid_values:
  - "A super weapon by its place in the `[SuperWeaponTypes]` list of the rules, counted from `0`, in the field after the type. The charge time goes in the last field, in frames."
caveats:
  - "A weapon that is charging keeps the time it has left. The new time applies from the next charge, and the weapon's sidebar clock and timer readout use it at once."
  - "The time replaces the weapon's `RechargeTime` for this house only, until Superweapon Reset Recharge Time puts it back."
  - "The time is kept even when the house does not hold the weapon yet, and applies once it does. A number off the list leaves the action doing nothing, and it fails."
  - "The time is kept in a [save game](/formats/save-games/)."
related:
  - type: action
    id: TACTION_SUPER_RESET_RECHARGE_TIME
  - type: action
    id: TACTION_SET_SUPER_CHARGE
  - type: system
    id: superweapons
---

```ini title="map file"
[Actions]
Faster=1,132,11,0,0,0,0,4500 ; the weapon at index 0 charges in 4500 frames
```
