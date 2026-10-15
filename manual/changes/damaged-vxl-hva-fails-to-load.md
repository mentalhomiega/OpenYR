---
title: Fail to load a VXL or HVA whose counts the file cannot back
category: fix
release: 0.2.0
targets:
- type: format
  id: vxl-hva
  effect: changed
credit:
- MentalHomiega
---

A `.VXL` or `.HVA` that is empty or counts more than the file holds now fails to load, and so does a `.HVA` that declares no frames or no layers. Before, such a file could make the game reserve gigabytes while it loaded, or crash it the first time the model was drawn. A `.VXL` whose layer names a missing info record, or whose span tables lie outside its voxel body, now fails as well.
