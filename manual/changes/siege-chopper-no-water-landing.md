---
title: Keep a Siege Chopper off water when it deploys
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

A Siege Chopper or other DeployToLand unit no longer lands on water or beach to deploy. Ordered to deploy over water, it looks at the eight cells around it for a clear land cell. If none is clear, the deploy is cancelled and the unit stays in the air on Guard. Before, it came down on the water and deployed there.
