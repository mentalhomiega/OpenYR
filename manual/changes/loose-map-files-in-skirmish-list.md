---
title: List loose multiplayer .MAP files in the skirmish map list
category: feature
release: 0.2.0
targets:
- type: system
  id: skirmish-setup
  effect: changed
credit:
- MentalHomiega
---

The skirmish and network map lists now include each loose `.MAP` file whose `[Basic]` section sets `MultiplayerOnly=yes`, under its `Name=`. A packet entry whose description is a string table label that no string resolves is left out of the list, so a mod's placeholder map no longer heads it.
