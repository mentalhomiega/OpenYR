---
key: CustomMusic
scope: client-settings
label: Custom music
when_omitted:
  kind: value
  value: "Both"
see_also: [IsScoreShuffle, IsScoreRepeat, ScoreVolume]
---

`CustomMusic=` in the `[Audio]` section of `RA2MD.INI` chooses which tracks the music list offers. The player's own tracks are the WAV, OGG, FLAC and MP3 files in the `Music` folder of the user directory. The game creates the folder at start-up if it is missing.

| Value | Music list |
| --- | --- |
| `Both` | The tracks that [THEME.INI](/formats/theme-ini/) declares, followed by the tracks in the `Music` folder. |
| `Stock` | The declared tracks only. The `Music` folder is ignored. |
| `Custom` | The tracks in the `Music` folder only. If the folder holds no playable track, the declared tracks play instead. |

The sound options screen sets the same value in its Custom music list. A value that is none of these is replaced with `Both`.

The game reads the folder when it starts and does not look again, so restart it after adding or removing files. A file that cannot be opened as audio, such as an empty or damaged one, is left out and named in the debug log. Files with other extensions and subfolders are ignored, and at most 500 tracks are used.

A custom track is titled with its file name less the extension, and the tracks follow the declared ones in file name order. Shuffle, repeat, the next and previous track commands, and the music volume treat them like any other track. A custom track is allowed for every side and mission. A scenario's `Theme=`, the menu music and the music trigger actions name declared tracks and play them whatever this setting says.

If a custom track fails to start during play, it is dropped from the list for the rest of the session.

The setting only changes the local music. It does not affect the match or other players.

```ini title="RA2MD.INI"
[Audio]
CustomMusic=Both
```
