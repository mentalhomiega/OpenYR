---
title: Let a saboteur route to a bridge repair hut across a movement zone boundary
category: fix
release: 0.2.0
targets:
- type: key
  id: BridgeRepairHut
  effect: changed
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

Before, a C4 soldier ordered onto a bridge repair hut dropped the order and never planted a charge when the hut's cell lay in another movement zone than the soldier. Soviet 3 (SOV03UMD.MAP) has such huts at 69,72 and 85,85. The route search now decides a saboteur's route to a structure, so the soldier walks into the hut, the charge goes off and the span beside the hut comes down. Other soldiers keep the zone refusal. Checked on SOV03UMD.MAP with a SEAL two cells west of the 69,72 hut and two cells east of the 85,85 hut.
