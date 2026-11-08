---
title: Glide the menu battle camera between screen pixels
category: fix
release: 0.2.0
targets:
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

The camera of the menu battle behind the title menus now pans smoothly. Before, the view moved one whole pixel at a time, about every two or three frames at 60 fps, and on a diagonal pan the horizontal and vertical steps landed on different frames. Now the battle is drawn at its exact camera position, and the part of a pixel the camera has moved past is applied when the frame is shown. The pan speed, the choice of what the camera follows and its easing are unchanged. Scrolling in games is unchanged.

While the camera is moving, the battle map is drawn with smooth filtering, so it looks slightly softer than when the camera is at rest.
