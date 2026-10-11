---
type: action
id: TACTION_RESTORE_STARTING_UNITS
title: "Restore Starting Units of..."
summary: "Puts back what a house had on the map when the scenario loaded."
valid_values:
  - "A house by number, either a country or a spawn house. `8997` names the trigger's own house."
caveats:
  - "The house's starting objects are its structures, vehicles, infantry and aircraft that stood on the map as the scenario finished loading. Objects that were carried inside another or were off the map are not among them."
  - "Each starting object is created again for the house, at full strength and facing north, in the cell it first stood on. A structure is not placed when a structure already fills the cell, so a structure that still stands is not doubled."
  - "Vehicles, infantry and aircraft that are still alive are not touched, and the new ones are added beside them. Running the action twice gives the house a second copy of each, as a vehicle waiting in its starting cell is joined by another."
  - "The action fails when the trigger has no house or the named house is not in play."
related:
  - type: action
    id: TACTION_RESTORE_STARTING_BUILDINGS
---

```ini title="map file"
[Actions]
Again=1,126,0,3,0,0,0,0,A ; the starting objects of the house at index 3 are back
```

A [save game](/formats/save-games/) keeps each house's list of starting objects.
