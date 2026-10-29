---
title: A teleporting soldier leaving a factory clears its rally cell
category: fix
release: 0.2.0
targets:
- type: key
  id: Teleporter
  effect: changed
credit:
- MentalHomiega
---

A soldier with `Teleporter=yes` that leaves a barracks and jumps to the rally point now moves the units standing on the rally cell aside, as in Yuri's Revenge. A near-target search for a soldier that cannot enter its target cell now uses the zone of the soldier's own destination cell.
