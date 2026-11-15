---
type: action
id: TACTION_AUXILIARY_POWER
title: Auxiliary Power
summary: Adds to the power a house always has, or takes it away with a negative amount.
valid_values:
  - The first number is the house, as the map's house list numbers it. The amount goes in the last field, where other actions put a waypoint, and may be negative.
caveats:
  - This is an Ares action, number 146. It is written with `11` in the first field, as in `146,11,<house>,0,0,0,0,<amount>`.
  - The amount is added to what earlier runs gave, so running the action twice adds twice. The total counts as power whatever the house's buildings make, and a spy's blackout does not remove it.
  - A negative total is a standing drain. A house whose buildings make less power than its buildings use, once the total is counted, is on low power.
  - The total is part of a saved game.
related:
  - type: action
    id: TACTION_GIVE_CREDITS
---

```ini title="map file"
[Actions]
Grid=1,146,11,2,0,0,0,0,5000 ; the house at index 2 gains 5000 power for good
Cut=1,146,11,2,0,0,0,0,-5000 ; and loses it again
```
