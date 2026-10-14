---
title: Place, select, message and delete multiplayer beacons
category: feature
release: 0.2.0
targets:
- type: command
  id: PlaceBeacon
  effect: changed
- type: command
  id: Delete
  effect: changed
- type: command
  id: ChatToAll
  effect: changed
- type: command
  id: ChatToAllies
  effect: changed
- type: key
  id: PlaceBeaconSound
  effect: added
credit: [MentalHomiega]
---

`PlaceBeacon`, bound to B by default, starts beacon mode in a multiplayer game. The next click on the map places a beacon on that cell, and a right click cancels the mode. Each house holds three beacons. A beacon shows on the map and the radar to its owner and to the owner's allies, with its message above it. Clicking a beacon selects it, and `Delete` removes the selected beacon when you own it.

With your beacon selected, Enter or Backspace writes its message in place of a chat line. Every player receives the message, and it shows only where the beacon is visible. A beacon an ally places shows a radar ping. `PlaceBeaconSound` under `[AudioVisual]` in `rules.ini` plays for the player who places a beacon and defaults to no sound. Single-player games place no beacons, as in Yuri's Revenge.
