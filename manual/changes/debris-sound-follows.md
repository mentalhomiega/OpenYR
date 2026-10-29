---
title: Let debris start sounds follow the debris
category: fix
release: 0.2.0
targets:
- type: system
  id: destruction-and-debris
  effect: changed
credit:
- MentalHomiega
---

Before, a voxel debris piece played its start sound once, at the place it was created. Now the sound moves with the piece while it flies and bounces, and it ends when the piece is removed. A start sound without a loop starts again each time it ends, until the piece is removed.
