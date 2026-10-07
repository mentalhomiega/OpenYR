---
type: action
id: TACTION_RESHROUD_AT
title: "Reshroud Map At..."
summary: "Shrouds the map again in a circle around a waypoint."
valid_values:
  - "A waypoint, by its number in the first field after the type. FinalAlert also writes the waypoint's letters in the last field, which this action does not read."
caveats:
  - "A circle of cells as wide as [`RevealTriggerRadius`](/keys/revealtriggerradius/) goes dark, the same circle that Reveal Around Waypoint opens."
  - "Every player's house is shrouded again except one that already sees the whole map."
  - "Objects of the house that stand inside the circle do not reveal it again until they next look around, which they do as they move."
related:
  - type: action
    id: TACTION_RESHROUD
  - type: action
    id: TACTION_REVEAL_SOME
  - type: system
    id: map-visibility
---

```ini title="map file"
[Actions]
Close=1,101,0,474,0,0,0,0,RG ; the map around waypoint 474 goes dark again
```
