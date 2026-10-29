---
title: Make a unit wait for its own reply to finish before it answers again
category: fix
release: 0.2.0
targets:
- type: key
  id: VoiceSelect
  effect: changed
- type: key
  id: VoiceMove
  effect: changed
- type: key
  id: VoiceAttack
  effect: changed
credit: [MentalHomiega]
---

A unit now says one reply at a time, as in Yuri's Revenge. An order or a selection puts its reply on the unit's own queue, and the reply starts when the unit has finished the one it was saying. A newer reply replaces one that is still waiting, and a reply identical to the one playing is dropped. Before, every click started a new reply on top of the last, so a unit clicked quickly spoke over itself. Different units still speak at the same time. Aircraft orders pick their reply the same way as other units: enter, harvest and the special orders use `VoiceEnter`, `VoiceHarvest` and `VoiceSpecialAttack`, and fall back to `VoiceMove`.
