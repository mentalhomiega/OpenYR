---
title: Match the display's refresh rate for motion smoothing by default
category: feature
release: 0.2.0
targets:
- type: key
  id: RenderFrameRate
  effect: changed
credit:
- MentalHomiega
---

[`RenderFrameRate`](/keys/renderframerate/) now defaults to `-1`, Match display. Solo and skirmish games draw between game frames at the display's refresh rate. Network games keep the smoothing off unless the file names a number. The Display page lists Match display beside Off. A settings file that already sets `RenderFrameRate` keeps its value.
