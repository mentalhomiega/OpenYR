---
title: Chrono Legionnaires board transports
category: fix
release: 0.2.0
targets:
- type: key
  id: Teleporter
  effect: changed
credit:
- MentalHomiega
---

A Chrono Legionnaire, or another soldier with `Teleporter=yes`, ordered into a friendly transport now jumps onto the transport's cell and boards it, as in Yuri's Revenge. It previously jumped to the nearest free cell beside the transport and repeated the jump every few frames without boarding.
