---
title: Leave DontScore objects out of the in-service count
category: fix
release: 0.2.0
targets:
- type: key
  id: DontScore
  effect: changed
credit: [MentalHomiega]
---

A structure, aircraft or infantryman with `DontScore=yes` no longer counts toward the in-service count of its type, so a structure-exists event does not see it, as in Yuri's Revenge. Before, such an object counted like any other. A vehicle with that key still counts when it enters service and stays counted after it leaves.
