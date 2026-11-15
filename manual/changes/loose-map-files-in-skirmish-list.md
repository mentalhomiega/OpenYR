---
title: List a mod's loose multiplayer .MAP files in the skirmish map list
category: feature
release: 0.2.0
targets:
- type: system
  id: skirmish-setup
  effect: changed
credit:
- MentalHomiega
---

With a mod active, the skirmish and network map lists now include each loose `.MAP` file whose `[Basic]` section sets `MultiplayerOnly=yes`, under its `Name=`. With a mod active, a packet entry whose description is a string table label that no string resolves is left out of the list, so a mod's placeholder map no longer heads it. Without a mod the lists are those of Yuri's Revenge.
