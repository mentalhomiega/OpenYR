---
type: action
id: TACTION_RESTORE_STARTING_BUILDINGS
title: "Restore Starting Buildings of..."
summary: "Puts back the structures the scenario placed for a house at the start."
valid_values:
  - "A house by number, either a country or a spawn house."
caveats:
  - "A structure counts as one of the house's starting structures when the map's `[Structures]` section placed it for that house. Structures it built, captured or gained from an action are not restored."
  - "A starting structure that still stands in its cell, with its type and its owner, is mended to full strength."
  - "A starting structure that is gone, or stands there under another owner, is placed again in its cell for the named house, without a build-up. When something blocks the cell, that structure is not placed."
  - "A house nobody plays leaves the action doing nothing, and it fails."
related:
  - type: format
    id: scenario-objects
  - type: action
    id: TACTION_CREATE_BUILDING
---

```ini title="map file"
[Actions]
Fix=1,130,0,12,0,0,0,0,A ; the starting structures of the house at index 12 are back
```

A [save game](/formats/save-games/) keeps each house's list of starting structures.
