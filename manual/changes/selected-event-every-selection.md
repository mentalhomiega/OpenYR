---
title: Raise the Selected event for every object selected
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_SELECTED
  effect: changed
credit: [MentalHomiega]
---

A tag's Selected event now fires for each object that is selected, whoever owns it and however many objects are selected together, as in Yuri's Revenge. Before, it fired only for the player's own objects, and in a group selection only for the first one.
