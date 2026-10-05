---
title: Read game files and rule overlays from mod folders
category: feature
release: 0.2.0
targets:
- type: format
  id: mod-ini
  effect: added
- type: format
  id: opents-ini
  effect: changed
- type: command
  id: launch:mod
  effect: added
credit:
- MentalHomiega
---

`Mods=` in `OPENTS.INI` and `-MOD=` on the command line choose mod folders, which the game searches for files ahead of its own directories. A mod's `mod.ini` can name rules, art and AI overlays that are read over the game's files before the map's overrides, and Reload rules reads the rules and art overlays again. The game does not check that multiplayer players run the same mods. Without mods, nothing changes.
