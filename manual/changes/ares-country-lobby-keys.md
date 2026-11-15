---
title: Read a country's loading theme, status text, taunt and observer files
category: feature
release: 0.2.0
targets:
- type: key
  id: LoadingTheme
  effect: added
- type: key
  id: MenuText.Status
  effect: added
- type: key
  id: File.Taunt
  effect: added
- type: key
  id: File.ObserverFlag
  effect: added
- type: key
  id: File.ObserverBackground
  effect: added
- type: key
  id: File.ObserverFlagAltPalette
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

A skirmish or network match now plays the `LoadingTheme=` of the local player's country, or of its side, while it loads. `MenuText.Status=` gives the country box of the skirmish and network lobbies a tip. `File.Taunt=`, `File.ObserverFlag=`, `File.ObserverBackground=` and `File.ObserverFlagAltPalette=` are read but nothing plays or draws them yet. The key names follow Ares. A match whose country and side set none of the keys behaves as before.
