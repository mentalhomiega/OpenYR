---
title: Take a shot's estimated damage off its target when it fires
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
- type: system
  id: firing-geometry
  effect: changed
credit:
- MentalHomiega
---

Each object now keeps an estimated health beside its strength. A shot that is not inaccurate takes its estimated damage off its target when it fires, and a unit that picks a target from a hunt scan, an area-guard scan or a scan for a nearby target while it moves or harvests takes the same estimate for its chosen weapon. Estimated health recovers toward strength one point on each frame whose count has bit 2 set. Target scans then see a target that is already due to die as having less health, so units spread their shots instead of all firing at it. gamemd's `TechnoClass::Fire` (0x6FDD50), `TechnoClass::TargetAndEstimateDamage` (0x709820) and `TechnoClass::Update` (0x6F9E50) set and restore the estimate, and `TechnoClass::ApplyDamage_Calc` (0x6FDB80) computes it. Before, a scan saw only a target's current strength.
