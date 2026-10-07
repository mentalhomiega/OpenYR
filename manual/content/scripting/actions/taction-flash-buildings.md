---
type: action
id: TACTION_FLASH_BUILDINGS
title: "Flash Buildings of Type..."
summary: "Flashes every structure of one type that the trigger's house owns."
valid_values:
  - "A structure's ObjectType ID, with the number of frames to flash in the last field."
caveats:
  - "Only structures of the trigger's house flash, wherever they are. One owned by another house, such as a neutral tech building, stays as it is."
  - "The structure flashes lighter on every other frame until the frames are used up."
  - "The action fails when the trigger has no house."
related:
  - type: action
    id: TACTION_FLASH_CAMEO
  - type: action
    id: TACTION_FLASH_TEAM
---

```ini title="map file"
[Actions]
Ping=1,131,9,CATHOSP,0,0,0,40 ; the house's hospitals flash for 40 frames
```
