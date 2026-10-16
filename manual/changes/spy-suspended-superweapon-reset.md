---
title: Empty the charge of a suspended superweapon after a spy enters its building
category: fix
release: 0.2.0
targets:
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

A spy that enters the building of a suspended superweapon now sets that weapon's countdown back to the full delay, so it shows no charge, and it stays suspended until power returns. We used to leave a suspended weapon with the countdown it had. A weapon that is charging still starts again from the beginning.
