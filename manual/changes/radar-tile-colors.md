---
title: Draw radar terrain colors as the original game does
category: fix
release: 0.2.0
targets:
- type: system
  id: map-visibility
  effect: changed
credit:
- MentalHomiega
---

Radar tile colors are now half as bright as before, and both halves of a cell share one color that does not change with height. A tree or other terrain object draws in a fixed light tan instead of its artwork's color. [What the radar draws](/systems/map-visibility/#what-it-draws) gives the order. HighRadarBrightness no longer affects the radar; [LowRadarBrightness](/keys/lowradarbrightness/) still scales the colors.
