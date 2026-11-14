---
title: Read the Ares vehicle thief keys BreakMindControl, OneTime and LeaveSound
category: feature
release: 0.2.0
targets:
- type: key
  id: VehicleThief.BreakMindControl
  effect: added
- type: key
  id: VehicleThief.OneTime
  effect: added
- type: key
  id: VehicleThief.LeaveSound
  effect: added
credit: [MentalHomiega]
---

A vehicle thief that takes a mind-controlled vehicle now frees it from its controller, so the controller cannot take it back later; `VehicleThief.BreakMindControl=no` stops such a thief from taking it at all. `VehicleThief.OneTime=yes` uses the thief up, so it does not step out of the wreck, and `VehicleThief.LeaveSound` plays when it does step out.
