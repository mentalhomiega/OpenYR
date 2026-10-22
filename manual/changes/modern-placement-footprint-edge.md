---
title: Show the placement footprint while zoomed and ignore releases over the panel
category: fix
release: 0.2.0
targets:
- type: system
  id: sidebar
  effect: changed
credit:
- MentalHomiega
---

A press on the map followed by a release over the command bar or the sidebar panel no longer places a pending building. Before this change, that release placed it under the panel.

When the map is zoomed in, the footprint and the placement range ring are no longer hidden while the pointer is outside the zoomed map rectangle, such as in the lower part of the screen. This applies to the classic layout too, and only while zoomed.

A footprint that reaches past the bottom edge of the map view is still cut off there, as in the original game.
