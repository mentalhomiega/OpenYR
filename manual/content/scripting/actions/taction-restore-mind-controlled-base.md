---
type: action
id: TACTION_RESTORE_MIND_CONTROLLED_BASE
title: "Restore Mind Controlled Base to..."
summary: "The trigger's house gives back the structures it took from a house with Mind Control Base."
valid_values:
  - "A house by number, either a country or a spawn house. `8997` names the trigger's own house."
caveats:
  - "Only structures the trigger's house took from the named house with [Mind Control Base](/mapping/actions/taction-mind-control-base/) go back. A structure the trigger's house has since captured, bought or lost to a third house is left alone."
  - "No capture announcement is played and no trigger attached to a structure for `Entered by...` springs."
  - "The action fails when the trigger has no house or the named house is not in play. It succeeds when there is nothing to give back."
related:
  - type: action
    id: TACTION_MIND_CONTROL_BASE
---

```ini title="map file"
[Actions]
Return=1,124,0,3,0,0,0,0,A ; the structures taken from the house at index 3 go back to it
```
