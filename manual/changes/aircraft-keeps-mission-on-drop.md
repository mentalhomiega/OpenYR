---
title: Keep a plane's mission when it drops a passenger
category: fix
release: 0.2.0
targets:
- type: system
  id: aircraft-operations
  effect: changed
credit:
- MentalHomiega
---

We no longer set a plane's own mission to Guard or Hunt when it drops a passenger by its weapon. The dropped soldier still takes its Guard or Hunt mission on landing. Before, that mission replaced the plane's own.
