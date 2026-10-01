---
title: Speak the Yuri's Revenge announcer
category: feature
release: 0.2.0
targets:
- type: system
  id: eva-speech
  effect: changed
- type: format
  id: eva-ini
  effect: added
- type: format
  id: opents-ini
  effect: changed
- type: action
  id: TACTION_PLAY_SPEECH
  effect: changed
- type: mission
  id: TMISSION_PLAY_SPEECH
  effect: changed
- type: key
  id: RechargeVoice
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit: [Lucas]
---

The announcer now speaks the lines EVAMD.INI lists, in the voice of the player's side, with each line's queue type and priority. Superweapons announce when they are ready, when they fire, and when an enemy builds one. Play speech numbers name a line's position in that file.
