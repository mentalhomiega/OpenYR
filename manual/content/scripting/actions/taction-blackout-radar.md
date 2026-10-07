---
type: action
id: TACTION_BLACKOUT_RADAR
title: "Blackout Radar..."
summary: "Cuts the trigger house's radar for a number of frames."
valid_values:
  - "A number of frames, in the field after the type."
caveats:
  - "The radar comes back when the frames are used up, if the house still has what it needs for one. This is the blackout a lightning storm causes."
  - "The action fails when the trigger has no house."
related:
  - type: system
    id: map-visibility
---

```ini title="map file"
[Actions]
Flicker=1,139,0,40,0,0,0,0,A ; the radar is dark for 40 frames
```
