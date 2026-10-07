---
type: action
id: TACTION_STOP_SOUNDS_AT
title: "Stop Sounds At..."
summary: "Ends the sounds Play Sound Effect At started at a waypoint."
valid_values:
  - "A waypoint, in the last field."
caveats:
  - "Every [placed sound](/systems/sound-effects/#placed-sounds) that Play Sound Effect At left in the waypoint's cell stops, including an endless loop. A loop that stops does not start again."
  - "Sounds that came from anywhere else, such as Play Sound Effect or an object's own sounds, carry on."
  - "A waypoint that does not exist leaves the action doing nothing."
related:
  - type: action
    id: TACTION_PLAY_SOUND_AT
---

```ini title="map file"
[Actions]
Quiet=1,116,0,686,0,0,0,0,ZK ; the sound placed at waypoint ZK stops
```
