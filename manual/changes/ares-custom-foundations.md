---
title: Read Ares custom building foundations
category: feature
release: 0.2.0
targets:
- type: key
  id: Foundation
  scope: buildingtype
  effect: changed
credit:
- MentalHomiega
---

`Foundation=Custom` gives a structure a footprint of any shape and size, listed cell by cell with the Ares keys `Foundation.X`, `Foundation.Y`, `Foundation.N`, `FoundationOutline.Length` and `FoundationOutline.N`. The radar draws such a structure at its full width and height. Saves made before this change cannot be loaded.
