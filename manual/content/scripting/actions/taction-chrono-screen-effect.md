---
type: action
id: TACTION_CHRONO_SCREEN_EFFECT
title: "Chrono Screen Effect for..."
summary: "Twirls the battlefield into white and back, as a chronoshift does."
valid_values:
  - "A number of frames, which is how long each swirl lasts. The picture stays white for 45 frames between the two swirls."
caveats:
  - "A number of `0` or less does nothing, and the action fails."
  - "The picture is swirled as it stood when the action ran, then whitened, then swirled back from the picture as it stands after the white. The swirl grows stronger and whitens during the first swirl, and unwinds and clears during the second."
  - "The player's input is locked while the effect runs, unless it was already locked before the action."
  - "The effect is not kept in a save game."
related:
  - type: action
    id: TACTION_TELEPORT_ALL_TO
---

```ini title="map file"
[Actions]
TimeEffect=1,127,0,40,0,0,0,0,A ; fades out and in over 40 frames each, and holds white for 45
```
