---
key: MovieVolume
summary: The volume of the sound in movies, as a fraction from 0 to 1.
see_also: [SoundVolume, VoiceVolume, UnitVoiceVolume]
when_omitted:
  kind: context-dependent
  note: The SoundVolume in effect when the game loads the settings. A settings file written before this key existed therefore plays movie sound at the sound effects level.
---

`MovieVolume` sets the level of the sound in every movie: briefings, intros, campaign movies, the movies the radar plays during a mission, and the menu movies. It covers both the VQA and the Bink movie formats. A VQA movie that is playing takes a change at once. A Bink movie keeps the level it started with.

At `0` or below, movie sound is silent. A Bink movie then plays without opening a sound device.

Values above `1` are read as `1`. A negative value is kept and saved back as written until the player presses OK in the sound options. OK stores the slider's position, which rounds the level to the nearest tenth and turns a negative value into `0`.

The sound options screen offers the fraction as a ten-step slider labelled Movies. Moving the slider changes the level at once and plays a beep at the new level. The level is saved to `RA2MD.INI` when the player leaves the options menu. During a game, the sound options return to the game controls dialog, and the level is saved when that dialog is accepted.

[`SoundVolume`](/keys/soundvolume/) no longer scales movie sound.
