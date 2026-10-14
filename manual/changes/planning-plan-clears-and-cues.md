---
title: Clear plans on the stock events and cue refused orders
category: feature
release: 0.2.0
targets:
- type: command
  id: PlanningMode
  effect: changed
credit: [MentalHomiega]
---

Outside planning mode, Scatter, Stop, deploy, detonate, repair, sell and archive clear the unit's planned orders, as in the stock game. In planning mode, Stop, Scatter, deploy and Guard are refused instead. A normal order clears them too, before its own checks, so a plan is cleared even when the order is then dropped. Primary, power on and power off clear the planned orders of the building they act on. A building and an aircraft take no planned orders in this build, so they change no unit's plan.

A planned order is refused when the unit already holds `MaxWaypointPathLength` queued orders, or when the unit has no plan yet and 128 of the player's units already hold one. A refused order plays `ScoldSound` and shows a message, at most once a frame. A queued planned order plays `AddPlanningModeCommandSound` for the local player.
