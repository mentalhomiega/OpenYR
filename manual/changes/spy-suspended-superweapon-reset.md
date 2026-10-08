---
title: Keep a full charge on a suspended superweapon after a spy enters its building
category: fix
release: 0.2.0
targets:
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

A spy that enters the building of a suspended superweapon now leaves that weapon with a full charge, and it stays suspended until power returns. We used to leave a suspended weapon with the charge it had. A weapon that is charging still starts again from the beginning.
