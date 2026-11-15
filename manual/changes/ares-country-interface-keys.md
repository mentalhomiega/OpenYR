---
title: Read a country's loading screen, flag and list position
category: feature
release: 0.2.0
targets:
- type: key
  id: File.LoadScreen
  effect: added
- type: key
  id: File.LoadScreenPAL
  effect: added
- type: key
  id: LoadScreenText.Name
  effect: added
- type: key
  id: LoadScreenText.SpecialName
  effect: added
- type: key
  id: LoadScreenText.Brief
  effect: added
- type: key
  id: LoadScreenText.Color
  effect: added
- type: key
  id: File.Flag
  effect: added
- type: key
  id: ListIndex
  effect: added
- type: system
  id: loading-screens
  effect: changed
- type: system
  id: skirmish-setup
  effect: changed
credit:
- MentalHomiega
---

A country's section can name the picture, palette, texts and text color of its multiplayer loading screen, the flag beside its player's name there, and its place in the lobbies' country lists. Before, only the ten stock countries had a Yuri's Revenge loading screen, and a country after them fell back to the older screen. The key names follow Ares.
