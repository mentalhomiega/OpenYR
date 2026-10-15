---
title: Speak the death voice only for the local player's objects
category: fix
release: 0.2.0
targets:
- type: key
  id: VoiceDie
  effect: changed
credit: [MentalHomiega]
---

An object the local player does not control no longer speaks its `VoiceDie` response when it is destroyed, as in Yuri's Revenge. Before, enemy objects spoke it as often as the player's own. Their `DieSound` plays for every owner, as before.
