---
type: action
id: TACTION_SUPER_RESET
title: "Superweapon Reset..."
summary: "Starts one of the trigger house's super weapons charging over from nothing."
valid_values:
  - "A super weapon by its place in the `[SuperWeaponTypes]` list of the rules, counted from `0`, in the field after the type."
caveats:
  - "A weapon that was ready is no longer ready. A weapon the house does not hold yet is left alone."
  - "A suspended weapon, one with its power out, is no longer ready and does not charge until the power returns."
  - "A number off the list leaves the action doing nothing, and it fails."
related:
  - type: action
    id: TACTION_SET_SUPER_CHARGE
  - type: system
    id: superweapons
---
