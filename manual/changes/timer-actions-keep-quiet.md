---
title: Quiet timer, lock input, resize and meteor actions
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_START_TIMER
  effect: changed
- type: action
  id: TACTION_STOP_TIMER
  effect: changed
- type: action
  id: TACTION_SET_TIMER
  effect: changed
- type: action
  id: TACTION_LOCK_INPUT
  effect: changed
- type: action
  id: TACTION_RESIZE_PLAYER_VIEW
  effect: changed
- type: action
  id: TACTION_METEOR_IMPACT
  effect: changed
- type: action
  id: TACTION_METEOR_SHOWER
  effect: changed
- type: action
  id: TACTION_TIMER_TEXT
  effect: changed
credit: [MentalHomiega]
---

Timer Start, Timer Stop, Timer Set, Lock Input, Resize Player View, Meteor Impact At and Meteor Shower At no longer ask EVA for an announcement, as in Yuri's Revenge. A mission that wants a line spoken at those moments uses the Play Speech action.

Timer Set now puts the mission timer's label back to the default each time it runs, so a label from Timer Text does not outlast the countdown it was set for.
