---
type: action
id: TACTION_FLASH_CAMEO
title: "Flash Cameo..."
summary: "Makes the sidebar cameo of an object type blink for a number of frames."
valid_values:
  - "An ObjectType ID of an infantry, vehicle, aircraft or structure, with the number of frames in the last field."
caveats:
  - "The cameo blinks white, six frames lit and six dark, until the frames have passed. Setting it again restarts the count."
  - "Only a cameo that is on the sidebar blinks, and only while its tab is shown. A type the house cannot build, or whose tab is not open, shows nothing."
  - "The action fails when no object type has the ID."
  - "The blinking is not kept in a save game."
related:
  - type: action
    id: TACTION_SET_TAB
  - type: system
    id: sidebar
---

```ini title="map file"
[Actions]
Hint=1,115,9,BFRT,0,0,0,120 ; the Battle Fortress cameo blinks for 120 frames
```
