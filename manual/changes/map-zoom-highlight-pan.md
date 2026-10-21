---
title: Keep HDR highlights on a zoomed map that scrolls
category: fix
release: 0.2.0
targets:
- type: system
  id: map-zoom
  effect: changed
credit: [MentalHomiega]
---

With HDR highlights on, a light on the map could lose its highlight on alternate frames while the zoomed map scrolled. The highlight now stays on through scrolling while the map is zoomed.
