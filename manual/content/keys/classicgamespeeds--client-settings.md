---
key: ClassicGameSpeeds
scope: client-settings
label: Game controls
see_also: [GameSpeed]
when_omitted:
  kind: value
  value: "no"
---
`ClassicGameSpeeds=yes` makes a campaign mission and a skirmish run at the frame lengths of Yuri's Revenge's own single player game. The key is read from the `[Options]` section of `RA2MD.INI`. The **Original game speeds** switch on the game controls dialog sets it, in a game and on the Game page of the settings.

Off, which is the default, a campaign and a skirmish use the same frame rates as a multiplayer game at every [`GameSpeed`](/keys/gamespeed/): 60, 45, 30, 20, 15, 12 and 10 frames a second for the values `0` to `6`. The campaign has its speed slider.

On, a skirmish uses these frame rates, and a campaign always runs at 31.25 frames a second with no speed slider:

| `GameSpeed` | `0` | `1` | `2` | `3` | `4` | `5` | `6` |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Skirmish frames a second | No limit | 62.5 | 31.25 | 20.83 | 15.62 | 12.5 | 10.41 |

For example, a skirmish at the Medium speed, `GameSpeed=3`, runs at 20 frames a second by default and at 20.83 with `ClassicGameSpeeds=yes`. At the Fastest speed, `0`, it runs at 60 by default and without a limit with the key on.

A multiplayer game ignores the key. The game speed also picks the length of some animations and EVA pauses, and it does that the same way with the key on or off.
