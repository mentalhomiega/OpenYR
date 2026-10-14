---
title: Bring down concrete low bridges when a repair hut's C4 goes off
category: fix
release: 0.2.0
targets:
- type: key
  id: BridgeRepairHut
  effect: changed
credit:
- MentalHomiega
---

Before, a weapon hit on a concrete low bridge did nothing, and a C4 charge on a bridge repair hut beside one left the span standing. Now a hit moves a piece to its cracked stage, and a second hit drops the three pieces across the span. A C4 charge on the hut brings down the concrete pieces around it. An engineer who repairs the span through the hut rebuilds the run with intact pieces, and the span is passable again. A unit standing on a piece that falls stays on it. Checked on SOV04DMD with a C4 charge and an engineer repair; other maps with concrete low bridges were not tested.
