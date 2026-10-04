---
title: Draw extra frames between game frames
category: feature
release: 0.2.0
targets:
- type: key
  id: RenderFrameRate
  effect: added
credit:
- MentalHomiega
---

The new `RenderFrameRate` under `[Video]` in `RA2MD.INI` sets how many pictures per second the game draws between game frames. A higher value shows moving objects in smoother motion; `0`, the default, draws once per game frame.
