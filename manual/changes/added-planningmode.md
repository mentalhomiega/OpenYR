---
title: Queue orders in planning mode
category: feature
release: 0.2.0
targets:
- type: command
  id: PlanningMode
  effect: added
credit: [MentalHomiega]
---

The PlanningMode command turns on planning mode while its key is held, and the sidebar's planning button toggles it. While planning mode is on, each order given to the player's selected units is queued instead of carried out. Releasing the key, or pressing the button again, starts the queues, and each unit carries out its orders one after another. A unit holds at most `MaxWaypointPathLength` queued orders (15 by default), and at most 128 of a player's units can hold a plan. A new order outside planning mode clears that unit's plan, and so do the unit commands that reset a unit, such as Stop and Scatter. While a plan is queued or running, the map shows its route: each queued destination has the waypoint cursor with its place in the queue, counting from 0, and the leg in progress has no number. Loading a save turns planning mode off, and plans are not saved.
