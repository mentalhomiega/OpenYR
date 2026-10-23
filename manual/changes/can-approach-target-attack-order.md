---
title: Let CanApproachTarget=no units chase an attack order
category: fix
release: 0.2.0
targets:
- type: key
  id: CanApproachTarget
  effect: changed
credit: [MentalHomiega]
---

A unit with `CanApproachTarget=no`, such as the Mirage Tank, now drives into range when ordered to attack a target out of reach, as in Yuri's Revenge. It still ignores out-of-range targets it picks for itself, except while a human player has it guard an area or a computer player has it hunt. A Magnetron holding a unit and a passenger of an open-topped transport also stop chasing targets out of reach.
