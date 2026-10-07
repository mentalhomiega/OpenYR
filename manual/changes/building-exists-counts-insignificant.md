---
title: Count insignificant buildings in Building exists and Building does not exist
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_BUILDING_EXISTS
  effect: changed
- type: event
  id: TEVENT_BUILDING_DOES_NOT_EXIST
  effect: changed
credit:
- MentalHomiega
---

The Building exists and Building does not exist events now count the buildings the house has at the moment, as in Yuri's Revenge, which includes buildings with `Insignificant=yes`. They counted without those, so a house that captured one of Allied mission 1's tech power plants was never seen to own it, and the mission could not go past powering up the time machine.
