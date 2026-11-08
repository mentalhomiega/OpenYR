---
title: Let a unit finish its current order before a committed plan starts
category: feature
release: 0.2.0
targets:
- type: command
  id: PlanningMode
  effect: changed
credit:
- MentalHomiega
---

Leaving planning mode starts a unit's plan at once only when the unit is on guard, or is attacking or guarding an area at a cell. A unit on any other order keeps it and starts the plan when it goes idle. A soldier walking to a transport still boards it, and a soldier inside a transport follows the plan once the transport unloads him. Before, the plan replaced the enter order and the soldier never boarded.

A unit that leaves a transport takes its next planned order after the unload has placed it, so the unload no longer discards the order.
