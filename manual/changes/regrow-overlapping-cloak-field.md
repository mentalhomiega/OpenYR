---
title: Regrow an overlapping cloak field after a collapse
category: fix
release: 0.2.0
targets:
- type: system
  id: cloaking
  effect: changed
- type: key
  id: CloakGenerator
  effect: changed
credit: [MentalHomiega]
---

When a cloak generator is destroyed, sold or loses power, the cells it shared with another generator lose their cloak for a moment, as in the original game. The other generator then regrows its field from its centre, so the shared cells are cloaked again within a few seconds. Only generators of the same house within two times the field radius plus 4 cells, with power and a finished field, regrow.
