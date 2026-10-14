---
title: Run an online game at its speed when the host is not the lowest seat
category: fix
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

An online game whose host is not the lowest seat no longer stays at 30 frames a second. The game speed setting, including a change made in game, now sets the frame rate on every machine.
