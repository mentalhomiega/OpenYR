---
title: Make an attack-move on an object pursue it
category: fix
release: 0.2.0
targets:
- type: command
  id: AttackMove
  effect: changed
credit:
- MentalHomiega
---

An armed attack-move, or Control with Shift, now also applies to a click on an enemy, a friendly vehicle or a ground attack, as in Yuri's Revenge. A click on an enemy gives each unit an attack-move that pursues it, taking any enemy within reach on the way. Control with Shift applies only when every selected object can attack-move, and the command arms only then. Control with Shift over an object no longer toggles the selection.
