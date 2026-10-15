---
title: Keep the skirmish and LAN lobby setup after a game
category: fix
release: 0.2.0
targets:
- type: system
  id: skirmish-setup
  effect: changed
credit: [MentalHomiega]
---

The skirmish lobby now opens with the setup it was left with. Before, a game that ended, or a cancel followed by another visit, brought the lobby back on the first map on hand, and its rows were cut to that map's start positions. The map, the rows and the game options are remembered, and the settings file keeps them across launches.

A hosted LAN game also opens on the map its host last picked. It used to open on the first map in the list.

A remembered map whose file is gone falls back to the first map on hand. The skirmish setup page lists the keys and the fallbacks.
