---
title: Hide garrison pips of an active enemy's structure
category: fix
release: 0.2.0
targets:
- type: system
  id: garrisons
  effect: changed
credit: [MentalHomiega]
---

A garrisonable structure of an active enemy house no longer shows its row of occupant pips. The player still sees the row on its own and allied structures, on structures of a house it spies on, and on structures of a house whose country sets `MultiplayPassive=yes`, such as an empty civilian structure. Soldiers may enter a structure only as before, so entry is unchanged. The other pips of a passive house's structure follow the same rule.
