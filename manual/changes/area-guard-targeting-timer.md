---
title: Make area guard wait for its targeting timer
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
- type: key
  id: GuardAreaTargetingDelay
  effect: changed
credit:
- MentalHomiega
---

An area-guarding object now scans for a target only when its targeting timer has run out. Each area scan restarts the timer with `GuardAreaTargetingDelay` plus a random 0 to 2 frames, whether or not it finds a target. Before, it scanned each time its mission ran. A target removed while more than 10 frames remain on the timer now shortens the wait to 4 to 8 frames.
