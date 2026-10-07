---
type: action
id: TACTION_EVICT_OCCUPIERS
title: "Evict Occupiers"
summary: "Forces the infantry out of the first structure the trigger is attached to."
caveats:
  - "Only the first structure whose tag holds this trigger is emptied. Attach the trigger to a single garrisoned structure, or use one trigger for each."
  - "Only a structure on the map counts. One in limbo is skipped."
  - "Each occupant steps out on the nearest free cell. An occupant with no room to stand is removed from the game."
  - "The action fails when no structure carries the trigger."
related:
  - type: action
    id: TACTION_MAKE_ELITE
  - type: system
    id: garrisons
---
