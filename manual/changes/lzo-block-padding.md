---
title: Load terrain packs that carry zero padding between or after their blocks
category: fix
release: 0.2.0
targets:
- type: format
  id: scenario-terrain
  effect: changed
credit:
- MentalHomiega
---

A scenario whose `[IsoMapPack4]` or `[IsoMapPack5]` section has zero padding after its last block, an empty block of two zero counts, or extra bytes after a block's compressed data used to be refused as damaged. It now loads in full, as gamemd loads it. A partial block header with a nonzero byte, and a block that expands to a size other than its header states, are still damage.
