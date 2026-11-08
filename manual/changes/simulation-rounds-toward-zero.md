---
title: Round the simulation toward zero, as the original game does
category: fix
release: 0.2.0
targets: []
credit: [MentalHomiega]
---

The game now rounds its floating-point results toward zero for the whole run, as the original game does. Before, the game switched to rounding to nearest after it read its first number from an INI file, so the last bit of many results differed from the original. Two builds playing the same skirmish stay in step for about 660 frames, then their simulations diverge.

A networked game needs every machine on this build. A machine on an earlier build does not simulate the same way and is expected to report a desynchronization.
