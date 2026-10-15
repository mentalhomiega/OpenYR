---
title: Empty a suspended superweapon's charge when a spy enters its building
category: fix
release: 0.2.0
targets:
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

A spy that enters the building of a suspended superweapon now empties that weapon's charge and leaves it not ready, and its countdown stays stopped until power returns. We used to leave a suspended weapon with the charge it had. A weapon that is charging still starts again from the beginning.
