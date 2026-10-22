---
title: Keep the placement footprint on screen at the bottom of the modern map
category: fix
release: 0.2.0
targets:
- type: system
  id: sidebar
  effect: changed
credit:
- MentalHomiega
---

In the modern layout the map reaches the bottom of the screen. A pending building's footprint hangs below the cell under the cursor, so near the bottom edge the footprint was cut off, and with the cursor on the last rows it was not visible at all. The footprint now moves up until its lowest row lies inside the map view. The building is placed on the cell where the footprint is drawn.

A press on the map followed by a release over the command bar or the sidebar panel no longer places a pending building. Before this change, that release placed it under the panel.

When the map is zoomed in, the footprint and the placement range ring are no longer hidden while the pointer is outside the zoomed map rectangle, such as in the lower part of the screen. This applies to the classic layout too, and only while zoomed.

At the default zoom, the classic layout is unchanged. Its footprint at the bottom edge of the map view is still cut off.
