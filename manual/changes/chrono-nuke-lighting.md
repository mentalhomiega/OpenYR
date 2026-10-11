---
title: Keep the nuclear lighting during a chronoshift screen effect
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_CHRONO_SCREEN_EFFECT
  effect: changed
credit: [MentalHomiega]
---

While a chronoshift screen effect runs, the map takes the nuclear flash lighting when a flash fades out, when a lightning storm starts or ends, and when a psychic dominator blast starts or ends, as Yuri's Revenge does. Before, those events set the storm's, the dominator's or the scenario's own lighting during the effect.
