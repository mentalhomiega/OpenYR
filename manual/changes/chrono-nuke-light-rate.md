---
title: Move the light at the nuclear rate during a chronoshift screen effect
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_CHRONO_SCREEN_EFFECT
  effect: changed
credit: [MentalHomiega]
---

While a chronoshift screen effect runs, the map's light moves toward its new level at the nuclear rate, `NukeAmbientChangeRate`, as Yuri's Revenge does. Before, a psychic dominator blast's rate or the scenario's own rate applied during the effect.
