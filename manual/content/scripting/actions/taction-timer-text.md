---
type: action
id: TACTION_TIMER_TEXT
title: "Timer Text..."
summary: "Sets the label the mission timer is shown under."
valid_values:
  - "A string table label, such as `MISSION:YuriTimer`. An action with no label takes the label away."
caveats:
  - "The label shows only while the mission timer runs. The timer is drawn at the bottom right of the battlefield, ahead of any super weapon timers, as the label and then the time left."
  - "The timer turns red once less than `TimerWarning` minutes are left."
  - "The label is kept in a [save game](/formats/save-games/) and returns after a load."
related:
  - type: action
    id: TACTION_SET_TIMER
  - type: action
    id: TACTION_START_TIMER
  - type: action
    id: TACTION_STOP_TIMER
---

```ini title="map file"
[Actions]
Countdown=3,27,0,482,0,0,0,0,A,103,4,MISSION:YuriTimer,0,0,0,0,A,23,0,0,0,0,0,0,A
```

The three actions set the timer to 482 seconds, give it the label, and start it.
