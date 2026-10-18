---
title: Mutate infantry only where a brute can stand
category: fix
release: 0.2.0
targets:
- type: key
  id: InfDeath
  effect: changed
- type: key
  id: InfantryMutate
  effect: changed
credit: [MentalHomiega]
---

A soldier killed by an `InfDeath=9` warhead now mutates only on ground infantry can walk on or on a bridge, in a cell with a free spot and no structure, as in Yuri's Revenge. Anywhere else he plays his explosion death sequence. The mutation animation is drawn in the attacker's house colors, and the new infantryman faces south-east instead of south.
