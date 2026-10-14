---
title: Let a BalloonHover unit join the playable area while it hovers
category: fix
release: 0.2.0
targets:
- type: key
  id: BalloonHover
  effect: changed
- type: system
  id: target-selection
  effect: changed
credit:
- MentalHomiega
---

A `BalloonHover=yes` unit that hovers over a cell inside the playable area now counts as having entered it, so computer units can target it and it can be selected. Before, only a move into a new cell marked it, so a hovering unit that started outside the playable area was ignored by computer targeting until it moved.
