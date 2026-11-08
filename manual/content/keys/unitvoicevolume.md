---
key: UnitVoiceVolume
summary: The volume of unit and building voice replies, as a fraction from 0 to 1.
see_also: [SoundVolume, VoiceVolume, MovieVolume]
when_omitted:
  kind: context-dependent
  note: The SoundVolume in effect when the game loads the settings. A settings file written before this key existed therefore plays unit replies at the sound effects level.
---

`UnitVoiceVolume` sets the level of the voice replies that units and buildings give: selection, movement and attack replies, the damage feedback and idle comments, the lines for deploying, entering, capturing and harvesting, and the death, crash and sinking lines. A reply already playing takes a change at once.

At `0` or below, unit replies are silent and are not started. Sound effects still play at their own level, as set by [`SoundVolume`](/keys/soundvolume/).

Values above `1` are read as `1`. A negative value is kept and saved back as written until the player presses OK in the sound options. OK stores the slider's position, which rounds the level to the nearest tenth and turns a negative value into `0`.

The sound options screen offers the fraction as a ten-step slider labelled Unit voices. Moving the slider changes the level at once and plays a sample reply from the first selection line of the GI (`E1`) in the rules, at the new level. Nothing plays when the rules give that infantry no selection line. The level is saved to `RA2MD.INI` when the player leaves the options menu. During a game, the sound options return to the game controls dialog, and the level is saved when that dialog is accepted.

The EVA announcements and taunts follow [`VoiceVolume`](/keys/voicevolume/), not this key.
