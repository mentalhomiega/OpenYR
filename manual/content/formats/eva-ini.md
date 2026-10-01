---
format_id: eva-ini
title: EVAMD.INI
summary: Lists the announcer's lines, the sample each side's voice plays for them, and how each line waits its turn.
kind: file
source_files:
- code/vox.cpp
- code/init.cpp
- code/audio/audioengine.cpp
related:
- type: system
  id: eva-speech
- type: action
  id: TACTION_PLAY_SPEECH
- type: format
  id: opents-ini
filenames:
- EVAMD.INI
---

`[DialogList]` names the announcer's lines, and each name is the section that describes that line. The game reads the file once at startup; [OPENTS.INI](/formats/opents-ini/#the-files-it-reads) can change its name. Without the file, the game speaks Tiberian Sun's `.AUD` speech files instead, as [EVA speech](/systems/eva-speech/) describes.

```ini title="EVAMD.INI"
[DialogList]
1=EVA_NuclearMissileReady
2=EVA_ConstructionComplete

[EVA_NuclearMissileReady]
Allied=ceva003
Russian=csof003
Yuri=cyur003
Type=QUEUE

[EVA_ConstructionComplete]
Allied=ceva048
Russian=csof048
Yuri=cyur048
Priority=LOW
```

A line's position in `[DialogList]`, counting the first as 0, is the number that [Play speech...](/mapping/actions/taction-play-speech/) and the [Play speech...](/scripting/missions/24/) team mission name. The order of the entries decides the positions; the numbers before the `=` do not. The game also asks for lines by name, so renaming a line it uses silences that announcement.

Each line's section takes these keys:

| Key | Meaning |
| --- | --- |
| `Allied`, `Russian`, `Yuri` | The sample each voice plays, without its `.WAV` extension. Only the first eight characters are used. |
| `Type` | How the line waits: `QUEUE`, `STANDARD`, `INTERRUPT` or `QUEUED_INTERRUPT`. Anything else is `STANDARD`. |
| `Priority` | `LOW`, `NORMAL`, `IMPORTANT` or `CRITICAL`. Anything else is `NORMAL`. |
| `Volume` | A multiplier for the line's volume. Without it the line plays at full volume. |

[EVA speech](/systems/eva-speech/#order) gives what `Type` and `Priority` do. `Text` is not read.

The voice is chosen by the player's side: the first side in `[Sides]` hears `Allied`, the second `Russian`, and any other `Yuri`. A campaign mission's `SpeechSide` in `[Basic]` picks the side instead. A line whose sample is empty for the voice in use says nothing.

A sample is looked up as a file first, so a loose or archived `ceva003.wav` is played in place of the bag's. Otherwise it is read from the sound bag, `AUDIO.BAG` through its `AUDIO.IDX`, where Yuri's Revenge keeps its announcer.
