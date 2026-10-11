---
type: action
id: TACTION_MIND_CONTROL_BASE
title: "Mind Control Base of..."
summary: "The trigger's house takes every structure of another house."
valid_values:
  - "A house by number, either a country or a spawn house. `8997` names the trigger's own house, which leaves the action with nothing to take."
caveats:
  - "Every structure the named house owns changes owner, together with what a structure carries or has docked. The structures keep their place, strength and veterancy, and their production queues are dropped."
  - "No capture announcement is played and no trigger attached to a structure for `Entered by...` springs."
  - "Each structure remembers the house it came from until [Restore Mind Controlled Base](/scripting/actions/taction-restore-mind-controlled-base/) returns it. Another house's mind control unit cannot take a structure while it is held this way."
  - "Vehicles, infantry and aircraft of the named house are not taken."
  - "The action fails when the trigger has no house or the named house is not in play."
related:
  - type: action
    id: TACTION_RESTORE_MIND_CONTROLLED_BASE
  - type: action
    id: TACTION_CHANGE_HOUSE
---

```ini title="map file"
[Actions]
Seize=1,123,0,3,0,0,0,0,A ; the trigger's house takes every structure of the house at index 3
```

A [save game](/formats/save-games/) keeps which house each taken structure came from.
