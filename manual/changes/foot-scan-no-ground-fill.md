---
title: Stop anti-air-only vehicles picking ground targets
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
credit:
- MentalHomiega
---

A scan that names no kind now takes only the kinds the object's weapons add. Before, a foot object whose scan named no ground kind also scanned infantry, vehicles and buildings, so a vehicle whose only weapon is [`AA=yes`](/keys/aa/) picked ground units. Such a vehicle now picks only aircraft. Aircraft, which add no kinds from their weapons, find nothing on that kind of scan.
