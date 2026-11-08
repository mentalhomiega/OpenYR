---
title: Add a volume slider for unit voice replies
category: feature
release: 0.2.0
targets:
- type: key
  id: UnitVoiceVolume
  effect: added
- type: key
  id: SoundVolume
  effect: changed
credit: [MentalHomiega]
---

Unit and building voice replies followed `SoundVolume`. They now follow `UnitVoiceVolume`, which the sound options screen sets with a new Unit voices slider. A settings file without the key takes the `SoundVolume` level, so replies keep their level until the slider moves.
