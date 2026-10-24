---
title: Search only the adjacent cells when a Siege Chopper cannot land
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

When a DeployToLand unit such as the Siege Chopper cannot land at the cell it is headed for, it now searches only the eight cells around it for a landing spot. Before, it flew to the nearest clear cell anywhere within 32 cells, which could take it far from where it was ordered to deploy. If none of the eight cells fits, the deploy is cancelled and the unit stays in the air on Guard.
