---
title: Refuse saves from builds that store games differently
category: feature
release: 0.2.0
targets:
- type: format
  id: save-games
  effect: changed
- type: format
  id: spawn-ini
  effect: changed
- type: command
  id: QuickLoad
  effect: changed
credit:
- MentalHomiega
---

A saved game now records a save revision, which changes whenever a build stores the game state differently. A save of the running version with another revision, or with none, is listed in the load dialog dimmed and marked `(Incompatible)`, and the game refuses to load it from the list, with [`QuickLoad`](/commands/quickload/) or from a launch file. Such a save used to be listed like any other and could fail partway through loading.
