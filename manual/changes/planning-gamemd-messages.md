---
title: Show gamemd's planning messages and refusals
category: feature
release: 0.2.0
targets:
- type: command
  id: PlanningMode
  effect: changed
- type: command
  id: StopObject
  effect: changed
- type: command
  id: ScatterObject
  effect: changed
- type: command
  id: DeployObject
  effect: changed
- type: command
  id: GuardObject
  effect: changed
credit:
- MentalHomiega
---

Planning mode shows the messages that gamemd shows, with gamemd's text. Turning planning mode on shows "You have entered waypoint mode. Release the waypoint key to exit." once per scenario. Turning it off shows "When you exit waypoint mode, your units will follow their waypoint path." once per scenario. These replace the earlier planning mode on and off messages.

In planning mode, Stop, Scatter, Deploy and Guard are refused. The refused command shows its message, plays the scold sound, and issues no order, so the unit's plan stays as it was. The messages are "Units in a waypoint path cannot be ordered to stop.", "Units in a waypoint path cannot be ordered to scatter.", "Units in a waypoint path cannot be ordered to deploy." and "Units in a waypoint path cannot be ordered to guard their current location."

An order that includes an aircraft, a building, or another object that cannot take a plan is refused as a whole. The message is "You cannot use waypoints with aircraft." for aircraft, "You cannot use waypoints with buildings." for buildings, and "You cannot use waypoints with one of these units." for other objects. Aircraft no longer take planned orders.

When a unit already holds 16 orders, the refusal shows "You have reached the maximum number of waypoints allowed for this path." When 128 units already hold plans, it shows "You have exceeded the maximum number of units allowed for all waypoint paths."

Refusals share one frame. A second refusal in the same frame shows no message and plays no scold sound. Each planning message stays on screen for 480 ticks, the duration gamemd gives them.
