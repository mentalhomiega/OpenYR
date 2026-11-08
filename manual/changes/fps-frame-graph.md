---
title: Draw a frame-time graph under the ShowFPS counts
category: feature
release: 0.2.0
targets:
- type: key
  id: ShowFrameGraph
  effect: added
credit:
- MentalHomiega
---

`ShowFrameGraph=yes` draws a graph of the last 120 gaps between presents under the `ShowFPS` counts. A tall column is a slow picture. The option is off by default, so the overlay keeps its two count lines unless a file turns the graph on.
