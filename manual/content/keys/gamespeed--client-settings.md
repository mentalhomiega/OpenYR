---
key: GameSpeed
scope: client-settings
label: Game controls
see_also: [ScrollRate, DetailLevel]
when_omitted:
  kind: value
  value: "3"
---
Each value holds the game to a frame rate, in frames per second. The game controls dialog names the values from `6` to `0` Slowest, Slower, Slow, Medium, Fast, Faster and Fastest. By default a campaign mission, a skirmish and a multiplayer game all use one table:

| Value | `0` | `1` | `2` | `3` | `4` | `5` | `6` |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Frames per second | 60 | 45 | 30 | 20 | 15 | 12 | 10 |

A multiplayer game runs at its value's rate or at the rate the slowest player's computer can sustain, whichever is lower. [`ClassicGameSpeeds=yes`](/keys/classicgamespeeds/) gives a campaign and a skirmish the frame lengths of Yuri's Revenge's single player game instead. A skirmish frame then lasts as many 16 ms units as the value is, and value `0` has no limit:

| Value | `0` | `1` | `2` | `3` | `4` | `5` | `6` |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Skirmish, `ClassicGameSpeeds=yes` | No limit | 62.5 | 31.25 | 20.83 | 15.62 | 12.5 | 10.41 |

With the key on, a campaign mission ignores the value: every frame lasts 32 ms, 31.25 frames per second, and the game controls dialog has no speed slider during a mission.

A frame that takes longer than its length is not made up by shorter frames after it, so a mission with slow frames plays below its rate and never above it.

The value also stretches some delays, more at faster speeds, so that they speed up less than the rest of the game. These are animations whose art sets [`Normalized=yes`](/keys/normalized/), a structure's construction and idle animations, infantry idle animations, and the pause between repeated EVA warnings that [`SpeakDelay`](/keys/speakdelay/) sets.

The game controls dialog offers `0` through `6` and saves the chosen value to `RA2MD.INI`. A value of `7` set in `RA2MD.INI` gives the same rate as `0`, or a skirmish frame of 112 ms with `ClassicGameSpeeds=yes`, though a recording made at that speed cannot be played back. A value below `0` is read as `0`, and a value above `7` as `7`, and the game writes the adjusted value back to `RA2MD.INI` when it next saves its settings. The skirmish and multiplayer setup screens replace the value with the speed chosen there, and a game started from [`SPAWN.INI`](/formats/spawn-ini/) uses the speed that file names. During a multiplayer game, a speed change from the game controls dialog, or from the in-game options screen in an Internet game, is sent as an order that changes the speed for every player.

:::note[Yuri's Revenge does not range-check GameSpeed]
Yuri's Revenge uses whatever number `RA2MD.INI` holds. With `-1` it crashes with a division by zero the first time it stretches a delay of five frames or more, and other values outside `0` to `7` stretch short delays by numbers read from outside its table. OpenYR holds the value to `0` through `7` instead, so a settings file that works in Yuri's Revenge also works here.
:::
