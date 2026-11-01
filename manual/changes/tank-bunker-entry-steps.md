---
title: Turn a vehicle south in a tank bunker before the walls rise
category: fix
release: 0.2.0
targets:
- type: system
  id: tank-bunkers
  effect: changed
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

A vehicle that reaches a Tank Bunker now stops on it while the bunker moves anything else off its cells, turns the vehicle toward its middle and drives it onto the middle, and turns its hull south. Only then do the walls rise and the vehicle is deselected, as in Yuri's Revenge. Before, the vehicle was set at the middle at once, still facing the way it came, and the walls rose immediately. Save games now store the entry step, so a save from an earlier build is refused.
