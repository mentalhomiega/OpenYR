---
type: action
id: TACTION_SET_SUPER_CHARGE
title: "Set Superweapon Charge..."
summary: "Sets how charged one of the trigger house's super weapons is."
valid_values:
  - "A super weapon by its place in the `[SuperWeaponTypes]` list of the rules, counted from `0`, in the field after the type. The charge goes in the last field, as a percentage from `0` to `100`."
caveats:
  - "The house has to hold the super weapon already. A weapon it does not have, a number off the list, or a percentage outside `0` to `100` leaves the action doing nothing, and it fails."
  - "The weapon takes the charge time it has now, so a time given by Superweapon Set Recharge Time counts. At `100` the weapon is ready at once, with no announcement."
  - "A weapon that is already ready stays ready, whatever the percentage."
related:
  - type: action
    id: TACTION_SUPER_SET_RECHARGE_TIME
  - type: action
    id: TACTION_SUPER_RESET
  - type: system
    id: superweapons
---

```ini title="map file"
[Actions]
Head start=1,129,11,0,0,0,0,70 ; the weapon at index 0 is 70 percent charged
```
