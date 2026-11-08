---
title: Keep HDR highlights while the unzoomed map scrolls
category: fix
release: 0.2.0
targets:
- type: system
  id: map-zoom
  effect: changed
credit: [MentalHomiega]
---

With HDR highlights on, the unzoomed map cleared the highlight mask on most frames while it scrolled, so a light on the map lost its highlight. The highlight now stays on through scrolling at the default view.
