---
title: Drop a high road span when a bridge repair hut's C4 goes off
category: fix
release: 0.2.0
targets:
- type: key
  id: BridgeRepairHut
  effect: changed
credit:
- MentalHomiega
---

Before, a C4 charge that went off on a bridge repair hut beside a road high bridge left the span standing when the intact deck was next to the hut. The charge now takes the deck down the way gamemd does, so the span falls in the same detonation, and units standing on the deck fall with it. The repair through the hut works as before. Checked on Deadman's east-west road span; north-south spans use the same code but were not tested.
