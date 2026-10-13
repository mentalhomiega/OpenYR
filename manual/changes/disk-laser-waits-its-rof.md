---
title: Wait the Floating Disc's ROF between laser shots
category: fix
release: 0.2.0
targets:
- type: key
  id: DiskLaser
  effect: changed
credit:
- MentalHomiega
---

A [`DiskLaser=yes`](/keys/disklaser/) weapon now waits for its `ROF`, with the usual rate modifiers, before it starts the next ring. Before, the next ring started as soon as the last one closed.
