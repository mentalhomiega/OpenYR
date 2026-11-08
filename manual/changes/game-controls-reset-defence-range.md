---
title: Update the Show defence range checkbox when game controls are reset
category: fix
release: 0.2.0
targets:
- type: key
  id: ShowDefenceRange
  effect: changed
credit: [MentalHomiega]
---

Reset to defaults on the game controls page now updates the Show defence range checkbox at once. Before, the box stayed ticked after a reset that turned the setting off, until another change redrew it.
