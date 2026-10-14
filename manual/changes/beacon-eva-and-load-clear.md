---
title: Announce beacons with the announcer and clear them on load
category: fix
release: 0.2.0
targets:
- type: command
  id: PlaceBeacon
  effect: changed
- type: command
  id: QuickLoad
  effect: changed
- type: system
  id: eva-speech
  effect: changed
- type: key
  id: PlaceBeaconSound
  effect: changed
credit:
- MentalHomiega
---

Placing a beacon now plays the announcer's `EVA_BeaconPlaced` line for you, before the placement sound. `PlaceBeaconSound` plays only for the player who placed the beacon, as its page now says. An ally who can see your beacon hears `EVA_BeaconDetected` when the beacon raises its radar ping. Both lines are read from EVAMD.INI, so a file without them plays nothing. Beacons are not written to saves, and loading a save now clears the beacons in play, as starting a scenario already did.
