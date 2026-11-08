---
title: Stop the map zoom at the map's visible area
category: fix
release: 0.2.0
targets:
- type: system
  id: map-zoom
  effect: changed
credit: [MentalHomiega]
---

On a small map the mouse wheel could zoom out far enough to show past the map's edge. The furthest zoom out now keeps the view within the map's visible area on both axes, so the visible area fills the screen along one axis at that zoom. If the visible area is smaller than the screen at normal size along either axis, the zoom out cannot go below normal size.
