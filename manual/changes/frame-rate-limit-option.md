---
title: Add a frame rate limit
category: feature
release: 0.2.0
targets:
- type: key
  id: FrameRateLimit
  effect: added
credit:
- MentalHomiega
---

The new `FrameRateLimit` under `[Video]` in `RA2MD.INI` caps the presents per second. It defaults to `-1`, which follows the display's refresh rate as before. `0` is unlimited. The Display page has the Frame rate limit list.
