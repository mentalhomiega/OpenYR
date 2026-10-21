---
title: Read the game mode's rules file over the rules
category: fix
release: 0.2.0
targets:
- type: format
  id: spawn-ini
  effect: changed
- type: key
  id: AlliesAllowed
  effect: changed
credit:
- MentalHomiega
---

A skirmish or network game now reads its mode's rules file, such as `MPDuelMD.ini`, over the game's rules, as Yuri's Revenge does. Megawealth, Duel, MeatGrinder, NavalWar, UnholyAlliance and TeamGame now play with their own unit, ore and prerequisite changes, and computer players may ally with each other in every mode except FreeForAll and Cooperative.
