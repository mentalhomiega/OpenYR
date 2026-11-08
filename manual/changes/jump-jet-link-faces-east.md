---
title: Start a jump-jet vehicle's flight facing east
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
credit: [MentalHomiega]
---

A Kirov or Floating Disc that leaves a War Factory now climbs out with its nose facing east, not north. Before, it kept facing north while it climbed and turned toward its destination only once it was high enough to do so.

The change applies to any jump-jet vehicle as it is created, so a vehicle that a script grants starts facing east too. A vehicle held by a Magnetron keeps its body facing, as before.
