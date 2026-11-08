---
title: Stop a Boomer from picking land targets on its own
category: fix
release: 0.2.0
targets:
- type: key
  id: LandTargeting
  effect: changed
credit: [MentalHomiega]
---

An object with `LandTargeting=2`, such as the Boomer, no longer picks a target on the ground off water when it looks for one on its own, as Yuri's Revenge does. Before, a Boomer idling at sea picked a tank on the shore as its target. An ordered attack on land still works, and the Boomer still picks ships on the water with its torpedoes.
