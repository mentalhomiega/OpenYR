---
title: LeaveBioReactorSound is read from object types
category: feature
release: 0.2.0
targets:
- type: key
  id: LeaveBioReactorSound
  effect: added
  scope: aircrafttype
credit:
- MentalHomiega
---

`LeaveBioReactorSound=` in an object type's entry is now read, as in Yuri's Revenge. It does not change the sound an absorbing structure plays: that stays the `[AudioVisual]` `LeaveBioReactorSound=`.
