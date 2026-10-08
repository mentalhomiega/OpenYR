---
title: Stop a drainer on foot from switching to Guard when its drain starts
category: fix
release: 0.2.0
targets:
- type: key
  id: DrainWeapon
  effect: changed
credit: [MentalHomiega]
---

When a drainer on foot starts draining a structure, it no longer switches to the Guard mission, as in Yuri's Revenge. We still clear its target and let it leave its team, as before.
