---
title: Free a dead slave miner's slaves to the Civilian-side house, or damage them when there is none
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

When a slave miner is destroyed, its field slaves now join the house whose side is Civilian, which is the Neutral house on the standard maps. Before, they joined the house named `Neutral`. With no killer and no Civilian-side house, each field slave takes C4 damage instead of keeping its owner. A slave docked in the miner is removed with it, and its kill is now credited to the destroyer.
