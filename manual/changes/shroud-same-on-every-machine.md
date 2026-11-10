---
title: Keep every human player's shroud the same on every machine of a network game
category: fix
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
- type: system
  id: map-visibility
  effect: changed
- type: format
  id: save-games
  effect: changed
breaking: false
credit:
- MentalHomiega
---

Each machine of a network game keeps every human player's shroud, and game logic reads it, for example a team sent to the nearest shrouded ground. Five things changed only the local player's shroud on each machine, as in Yuri's Revenge, so the machines could disagree about another player's shroud: a gap generator, a spy in a radar, a placed `RevealToAll=yes` structure, the look of moving aircraft and vehicles in the air, and objects entering a playable area that a trigger enlarged. Each now changes the shroud of every human player it applies to, on every machine. What each player sees, and every campaign and skirmish, play as before. Save games now store which players each gap generator shrouds, so a save from an earlier build is refused.
