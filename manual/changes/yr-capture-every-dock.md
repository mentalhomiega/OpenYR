---
title: Capture every aircraft docked at a captured pad
category: fix
release: 0.2.0
targets:
- type: key
  id: NumberOfDocks
  effect: changed
credit: [MentalHomiega]
---

Capturing a structure with several docks now captures each object standing on its dock, not only the one on the first dock. An object not standing on its dock is still sent away. A structure with one dock behaves as before.
