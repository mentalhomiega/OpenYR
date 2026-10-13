---
title: Clear a bridge repair hut's C4 charge when it goes off
category: fix
release: 0.2.0
targets:
- type: key
  id: BridgeRepairHut
  effect: changed
credit:
- MentalHomiega
---

Before, a C4 charge planted on a `BridgeRepairHut=yes` structure stayed armed. The hut is immune to ordinary damage, so the charge's blast was refused every time. Now the charge clears when its timer runs out, and the hut stays standing. The bridge search runs beside the hut at that moment, and an Ivan bomb that goes off on the hut runs it too. A span the search can bring down comes down.
