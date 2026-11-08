---
title: Destroy a chronoshifted unit that lands outside the playable area when it lands
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A unit that a chronosphere sends to a spot outside the playable area no longer dies when the chronosphere fires. It waits 60 frames, leaves, and is destroyed when it lands, as in Yuri's Revenge. A unit, soldier or Chrono Legionnaire whose wait or hold ends without a target also shortens its wait to the next area scan to 4 to 8 frames when more than 10 remain, looks for a target at once, and goes idle if it finds none.
