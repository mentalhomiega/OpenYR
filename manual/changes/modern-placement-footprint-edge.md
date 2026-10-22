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

The classic layout is unchanged, including its footprint at the bottom edge of the map view.
