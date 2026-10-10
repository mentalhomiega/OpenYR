---
type: action
id: TACTION_JUMP_CAMERA_HOME
title: "Jump Camera Home"
summary: "Moves the view to the local player's base at once."
caveats:
  - "The view moves on the computer that runs the action, whichever house owns the trigger."
  - "It does what the [`CenterBase`](/commands/centerbase/) key does, including ending follow mode. The view stays where it is when the local player has no structure and no vehicle from `BaseUnit`."
related:
  - type: command
    id: CenterBase
  - type: action
    id: TACTION_JUMP_CAMERA
---

```ini title="map file"
[Actions]
Home=1,145,0,0,0,0,0,0,A ; the view jumps to the player's base
```
