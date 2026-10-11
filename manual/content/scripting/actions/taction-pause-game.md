---
type: action
id: TACTION_PAUSE_GAME
title: "Pause Game..."
summary: "Freezes the game for a number of real seconds."
valid_values:
  - "A number of seconds, in the first field after the type. `0` or less does nothing."
caveats:
  - "Only a campaign or skirmish game freezes. In a game against other players the action does nothing."
  - "While the game is frozen no object moves, no trigger springs and the mission clock stops. The view stays drawn, input is ignored and the mouse pointer is hidden."
  - "The seconds are real time, not game time, so game speed does not change the length. An action that runs while the game is already frozen starts the count again from the new number instead of adding to it."
  - "The action succeeds whichever house owns the trigger."
related:
  - type: action
    id: TACTION_LOCK_INPUT
  - type: action
    id: TACTION_PLAY_INGAME_MOVIE_PAUSED
---

```ini title="map file"
[Actions]
Wait=1,110,0,5,0,0,0,0,A ; the game freezes for five seconds
```
