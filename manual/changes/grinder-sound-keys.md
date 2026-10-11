---
title: The grinder sound keys are read from rules and object types
category: feature
release: 0.2.0
targets:
- type: key
  id: EnterGrinderSound
  effect: added
  scope: aircrafttype
- type: key
  id: LeaveGrinderSound
  effect: added
credit:
- MentalHomiega
---

`EnterGrinderSound=` in an object type's entry and `LeaveGrinderSound=` in `[AudioVisual]` and in object types are now read, as in Yuri's Revenge. Neither changes which sound a grinder plays: it still plays the `[AudioVisual]` `EnterGrinderSound=`.
