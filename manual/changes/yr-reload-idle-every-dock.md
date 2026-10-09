---
title: Wake an idle rearming pad for any docked aircraft
category: fix
release: 0.2.0
targets:
- type: key
  id: UnitReload
  effect: changed
credit: [MentalHomiega]
---

An idle `UnitReload=yes` pad with several docks now starts rearming when any docked aircraft needs it. Before, only the aircraft on the first dock could wake it. A pad with one dock behaves as before.
