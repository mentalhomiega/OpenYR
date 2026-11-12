---
title: Add RenderThread to run the graphics device on its own thread
category: feature
release: 0.2.0
targets:
- type: key
  id: RenderThread
  effect: added
credit:
- MentalHomiega
---

[`RenderThread`](/keys/renderthread/) moves the graphics device's work and the present to a thread of its own. The default `no` changes nothing.
