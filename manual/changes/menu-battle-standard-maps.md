---
title: Pick only standard maps for the menu battle
category: fix
release: 0.2.0
targets:
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

The battle behind the menus now picks only maps made for the standard mode: a map that lists game modes must list `standard`, and the two furthest start positions must lie at least 0.35 of the map's visible diagonal apart. This leaves out the Megawealth, Duel, Team Game and cooperative maps, and maps such as XRound and MojoSprt whose players start round one field in the middle, where the computers could not build. With a mod that declares lobby modes, the mod's own Standard mode decides. A map named by `Map=` under `[MenuBattle]` is still used whatever its mode.
