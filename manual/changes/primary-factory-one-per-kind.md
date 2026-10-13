---
title: Keep one primary factory per kind
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
- type: key
  id: IsMobileWar
  effect: changed
credit: [MentalHomiega]
---

A house now has at most one primary factory per kind, and a naval yard is a separate kind from vehicle factories. Before, a newly placed factory was flagged primary when its house owned more than one structure of the eighth BuildingType, so one kind could have several primaries. Now a new factory is primary only when its kind has none. Toggling clears only the same kind. A structure that changes hands by capture or mind control loses its flag, and its new owner flags it only when it has no primary of its kind. An `IsMobileWar=yes` structure takes the flag when it first opens, unless it already has it.
