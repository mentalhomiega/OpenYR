---
title: Zoom-out limit
category: feature
release: 0.2.0
targets:
- type: key
  id: ZoomOutView
  effect: added
credit: [MentalHomiega]
---

The mouse wheel no longer zooms out past the view the original game shows at 2560x1440, and [`ZoomOutView=4K`](/keys/zoomoutview/) widens that to the view at 3840x2160, which no setting exceeds. Scripted scenes keep their own camera zoom, and `DebugMode=yes` in a single-player game lifts the limit. Clicking and drag-selecting work at every zoom: the list of clickable objects holds more of them in a zoomed-out view, and the click search reaches at least seven frame pixels from an object.
