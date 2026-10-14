---
title: Match planning order limits and post-order messages to gamemd
category: feature
release: 0.2.0
targets:
- type: command
  id: PlanningMode
  effect: changed
credit:
- MentalHomiega
---

A unit's plan holds 16 orders, as in gamemd. Before, the limit followed `MaxWaypointPathLength`, which now sets only the length of a waypoint path. The sixteenth order shows "You have reached the maximum number of waypoints allowed for this path." once its frame has run, and a seventeenth order is refused with that message.

An enter or capture order ends a plan, and so does an attack or guard area order at a cell. Once a plan has one of these as its last order, a later order is refused with "Your last order to this unit terminated the waypoint path." The same message shows once the frame that queues one of these orders has run.

A planning click that would give plans to more than 128 of the player's units is refused as a whole, with "You have exceeded the maximum number of units allowed for all waypoint paths." No unit in the selection takes a plan from that click.
