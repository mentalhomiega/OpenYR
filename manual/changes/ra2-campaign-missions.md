---
title: Start the Red Alert 2 campaign missions
category: fix
release: 0.2.0
targets:
- type: key
  id: Scenario
  scope: campaign
  effect: changed
- type: key
  id: Description
  scope: campaign
  effect: changed
- type: format
  id: opents-ini
  effect: changed
credit:
- MentalHomiega
---

A campaign whose `Scenario=` names a Red Alert 2 map with the `RA2->` prefix now starts that map. It used to look for a file named with the prefix, which does not exist, so the mission did not start.

Red Alert 2's missions index their movies by Red Alert 2's own movie list, and Yuri's Revenge's list has fewer entries. A mission that played an in-game movie past the end of that list closed the game. Those movies now play by name from Red Alert 2's list, read from the art file `RA2Art=` names in `OPENTS.INI`. A movie that neither list names plays nothing.

The campaign list leaves out a campaign whose `Scenario=` names no map, such as a header row or a movie sequence. Before, picking one of those rows failed to start.
