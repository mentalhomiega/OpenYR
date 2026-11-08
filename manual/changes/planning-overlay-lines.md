---
title: Draw planned orders as gamemd's planning overlay
category: fix
release: 0.2.0
targets:
- type: command
  id: PlanningMode
  effect: changed
credit: [MentalHomiega]
---

Planned orders are drawn only while planning mode is on, as in Yuri's Revenge, and no longer with the waypoint cursor and a number at each stop. Each leg from a unit through its queued stops is a black line four pixels thick, white for a selected unit, with a two-pixel dashed line in the owner's colour marching along it: dim until the plan runs, brighter once it does. Each stop has a small filled square. A guard area or attack move leg longer than 36 pixels carries a box with that order's cursor at its middle.
