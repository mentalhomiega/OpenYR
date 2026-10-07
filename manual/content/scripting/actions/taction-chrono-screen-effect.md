---
type: action
id: TACTION_CHRONO_SCREEN_EFFECT
title: "Chrono Screen Effect for..."
summary: "Whites out the battlefield and fades it back in, as a chronoshift does."
valid_values:
  - "A number of frames, which is how long each fade lasts. The picture stays white for 45 frames between the two fades."
caveats:
  - "A number of `0` or less does nothing, and the action fails."
  - "Yuri's Revenge also twirls the picture and locks the player's input while the effect runs. This engine only fades the picture, and the player stays in control."
  - "The effect is not kept in a save game."
related:
  - type: action
    id: TACTION_TELEPORT_ALL_TO
---

```ini title="map file"
[Actions]
TimeEffect=1,127,0,40,0,0,0,0,A ; fades out and in over 40 frames each, and holds white for 45
```
