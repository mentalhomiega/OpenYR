---
title: Keep drawing an aircraft from changing the game
category: fix
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
breaking: false
credit:
- MentalHomiega
---

Drawing an aircraft no longer changes the game. To draw its shadow on the ground, the game moved the aircraft down to the ground and back, and each move took it off the map and put it back. That revealed the aircraft to every player who had not yet discovered it, which also gives its owner a look around, and reordered the objects of its cell. Each machine of a network game did this only for the aircraft on its own screen, at its own time, so the machines could disagree about the shroud and about what an aircraft had been discovered by. Yuri's Revenge draws aircraft the same way. The shadow is now drawn without moving the aircraft.
