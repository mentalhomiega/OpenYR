---
title: Limit UnitLost.Suppress to the kill its warhead caused
category: fix
release: 0.2.0
targets:
- type: key
  id: UnitLost.Suppress
  effect: changed
credit:
- MentalHomiega
---

A giant squid that sinks a victim in the red band no longer inherits the [`UnitLost.Suppress`](/keys/unitlost.suppress/) of the last warhead that hit the victim. Before, a victim left in the red band by a suppressing warhead was sunk without the unit-lost announcement. The sinking now counts as a kill by the squid's own warhead, so only that warhead's setting applies.
