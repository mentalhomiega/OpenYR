---
title: Speak the feedback voice only for the local player's objects
category: fix
release: 0.2.0
targets:
- type: key
  id: VoiceFeedback
  effect: changed
credit:
- MentalHomiega
---

An enemy object no longer speaks its `VoiceFeedback` response when it drops below half strength, as in Yuri's Revenge. Before, enemy objects spoke it as often as the player's own. Their `DamageSound` and `DieSound` play for every owner, as before.
