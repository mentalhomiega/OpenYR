---
title: Take the first warhead effect in gamemd's order
category: fix
release: 0.2.0
targets:
- type: key
  id: Airstrike
  effect: changed
- type: key
  id: EMEffect
  effect: changed
- type: key
  id: Webby
  effect: changed
credit: [MentalHomiega]
---

A warhead that sets several of these flags now uses the first one in gamemd's order: MindControl, IvanBomb, ElectricAssault, Parasite, Temporal, IsLocomotor, Airstrike, DirectRocker, BombDisarm, MakesDisguise, then NukeMaker. Before, Airstrike, NukeMaker and EMEffect were checked first, so an Airstrike warhead that also set MindControl called planes. EMEffect and Webby now act only when no other flag does. ElectricAssault still lets Airstrike call planes on a target that is not an overpowerable building.
