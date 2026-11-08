---
title: Depth-test tiled animations as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: key
  id: Tiled
  effect: changed
credit:
- MentalHomiega
---

Each copy of a tiled animation, such as the ion cannon beam, is now tested against the depth buffer, so a tall structure in front of the beam hides the part behind it. The beam was drawn over everything before.
