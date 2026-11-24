---
title: Count overlapping sensor fields
category: fix
release: 0.2.0
targets:
- type: key
  id: SensorArray
  effect: changed
credit: [ZivDero]
---

A cell covered by two sensor arrays of one house now stays sensed as long as one of them still covers it. Before, when one array was taken off the map, the shared cells stayed sensed only if the other array was operational at that moment.

Capturing a sensor array now removes its coverage from the old owner and, if the array is operational at that moment, gives it to the new owner. Before, the old owner kept sensing the array's cells for the rest of the game, and the new owner sensed nothing until another array was removed.
