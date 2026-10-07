---
title: Read what a Yuri's Revenge map names in an action
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_TEXT_TRIGGER
  effect: changed
- type: action
  id: TACTION_PLAY_SOUND
  effect: changed
- type: action
  id: TACTION_PLAY_MUSIC
  effect: changed
- type: action
  id: TACTION_PLAY_SPEECH
  effect: changed
credit: [MentalHomiega]
---

A Yuri's Revenge map names the string table label, sound, theme or EVA line of these actions rather than numbering it. The actions used to read the name as the number 0 and acted on the first entry of each list; they now show the label, play the sound, queue the theme and speak the line the map names.
