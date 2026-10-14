---
title: Clamp a structure's laser depth adjust to zero or below
category: fix
release: 0.2.0
targets:
- type: system
  id: firing-geometry
  effect: changed
credit:
- MentalHomiega
---

A structure's laser beam sorts against its render point by how far its mounting is drawn above that point. That adjust is now never positive, so a mounting drawn below the render point gives zero. An object that is not a structure now uses zero for its laser beam; before, its adjust followed its mounting's screen height too.
