---
type: action
id: TACTION_SET_EVA_VOICE
title: Set EVA Voice
summary: Changes the player's EVA voice, or switches EVA off.
valid_values:
  - "`0` is the Allied voice, `1` the Russian voice, `2` the Yuri voice, and a higher number the voice at that place in the `[EVATypes]` list after them. `-1` switches EVA off."
caveats:
  - This is an Ares action, number 148. A number that names no voice does nothing.
  - A voice stays until the scenario ends or another voice is chosen. A saved game keeps it.
  - `-1` has the effect of [Disable Speech](/mapping/actions/taction-disable-speech/), and a voice number switches EVA on again, as [Enable Speech](/mapping/actions/taction-enable-speech/) does.
related:
  - type: action
    id: TACTION_DISABLE_SPEECH
  - type: action
    id: TACTION_ENABLE_SPEECH
  - type: system
    id: eva-speech
---

```ini title="map file"
[Actions]
Voice=1,148,0,1,0,0,0,0,A ; EVA speaks with the Russian voice from now on
Mute=1,148,0,-1,0,0,0,0,A ; EVA falls silent
```
