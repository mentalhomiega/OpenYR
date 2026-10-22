---
key: SoundVolume
summary: The volume of sound effects, as a fraction from 0 to 1.
see_also: [VoiceVolume, ScoreVolume, SoundLatency, UnitVoiceVolume, MovieVolume]
when_omitted:
  kind: value
  value: ".7"
---

`SoundVolume` sets the level of every sound effect, including the sounds of the menus and the score screen. The fraction multiplies the final volume of every sound effect, including sounds already playing.

Unit and building replies follow [`UnitVoiceVolume`](/keys/unitvoicevolume/), and movie sound follows [`MovieVolume`](/keys/movievolume/), so neither changes with this key. A settings file that omits one of those two keys takes its level from this key.

At `0` or below, sound effects are silent.

Values above `1` are read as `1`. A negative value is kept and saved back as written until the player presses OK in the sound options. OK stores the slider's position, which rounds the level to the nearest tenth and turns a negative value into `0`.

The sound options dialog offers the fraction as a ten-step slider. Moving the slider changes the level at once and plays a beep. The level is saved to `RA2MD.INI` when the player leaves the options menu. During a game, the sound options return to the game controls dialog, and the level is saved when that dialog is accepted.
