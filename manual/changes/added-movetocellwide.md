---
title: Move to Cell reads 128 * Y + X unless rules ask for the wide form
category: fix
release: 0.2.0
targets:
- type: key
  id: MoveToCellWide
  effect: added
- type: mission
  id: TMISSION_MOVECELL
  effect: changed
credit: [MentalHomiega]
---

A Move to Cell script line reads its cell as `128 * Y + X`, as Yuri's Revenge does, unless the rules declare Ares armor types or `MoveToCellWide=yes` asks for `1000 * Y + X`. Before, every map with `NewINIFormat=4` was read the wide way.
