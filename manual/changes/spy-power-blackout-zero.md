---
title: End a running blackout when a spy enters a power plant with a zero duration
category: fix
release: 0.2.0
targets:
- type: key
  id: SpyPowerBlackout
  effect: changed
credit:
- MentalHomiega
---

With `SpyPowerBlackout=0`, a spy that enters a power plant now ends the owner's blackout that is already running, as in Yuri's Revenge. We used to do nothing at zero. Positive durations work as before.
