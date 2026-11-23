---
title: Chrono Screen Effect with no frames succeeds
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_CHRONO_SCREEN_EFFECT
  effect: changed
credit:
- MentalHomiega
---

Action 127 with a number of frames of `0` or less starts no effect and now counts as carried out, as in Yuri's Revenge. It failed before.
