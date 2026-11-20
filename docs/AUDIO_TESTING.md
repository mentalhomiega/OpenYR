# Audio testing

We test sound with three parts: an audio log of every sound the game asks for and what became of it, a WAV
recording of the mix made while the test run stays muted, and `scripts/audio_check.py`, which checks the recording
against the log. The same tool also finds known sounds in a recording made outside the game, such as a play-tester's
video.

None of this changes the game. The log and the recording read sound state only, and both are off unless a test
script turns them on, so regress logs stay the same.

## Recording a run

An autotest script turns the parts on with these steps ([AUTOTEST.md](AUTOTEST.md#audio)):

```
10 sounds 2
10 audiocapture my-test
20 volume voice 1
20 volume sound 1
40 speak EVA_UnitReady
500 audiocapture stop
510 sounds 0
520 quit
```

- `audiocapture <name>` starts the recording and the audio log. They go to `AudioTest\<name>.wav` and
  `AudioTest\<name>.log` in the run's user folder, for example `build\agent29-autotest-user\AudioTest\`.
- `audiocapture stop` ends both and writes every sound and stream the recording used, decoded to WAV, into
  `AudioTest\<name>.sounds\`. The tool matches against these copies.
- `sounds 2` also copies each audio log line into the debug log with an `AUDIOLOG ` prefix, so a run's `-Filter`
  can show them and regress can compare them. `sounds 2` without `audiocapture` gives the log lines alone.
- `run_autotest.ps1` sets every volume option to 0. Raise the ones the test needs with `volume` after
  `audiocapture`, or nothing reaches the recording.

Run the script as any other:

```
powershell -File scripts/run_autotest.ps1 -Build agent29 -Script src-agent29/tests/autotest/audio-eva-three.txt -Width 800 -Height 600 -Seconds 120 -Uncapped
```

### How the recording works

While `audiocapture` runs, the sound device is stopped and the game thread mixes the sound itself, one game frame of
audio for each game frame: 1/45 s at game speed 1, as a solo game is paced. The recording therefore follows game
frames, not the wall clock, and an uncapped or slow run gives the same audio. The audio clock the announcer's timers
use follows the recorded audio too.

The recording is the full mix at the volume options' levels. The test run's master mute is lifted for the recording
only; the device stays stopped, so the run is still silent.

Two runs of the same script give byte-identical WAV and log files when `audiocapture` comes before the first sound,
as in the example. A sound that started before the recording is listed as `live` at its start and is timed by the
wall clock until then, so a recording that begins mid-sound can differ between runs.

### Limits of the recording

- A movie pauses the recording. The log writes `capture pause reason=movie` and `capture resume`, and the tool flags
  the pause.
- A wait that does not run game frames, such as a menu or a dialog box, adds no audio to the recording.
- When the frame counter jumps, as when a saved game loads, the recording adds at most 60 frames of audio for the
  jump.

## The audio log

Each line is `f=<game frame> ms=<audio clock> pos=<sample position in the recording> <kind> <action> key=value...`.
`pos` is present only while recording. Names with spaces are quoted.

| Kind | Actions | What it records |
| --- | --- | --- |
| `sfx` | `request`, `silent`, `place`, `dropped` | A sound effect asked for: name, group, level, pan, and for a placed sound its cell and range. `silent` is a placed sound out of range; `dropped` gives the reason, such as `volume-zero` |
| `event` | `started`, `delayed`, `queued`, `refused-*`, `cut-limit`, `cut-budget`, `stopped`, `finished`, `live`, `retarget` | What the mixer did with a sound: handle, type, group, priority, level, gain, pan, pitch, the sample files chosen and loop points. `retarget` is a level or pan change with its ramp |
| `stream` | `open`, `live`, `stop`, `refused` | Streamed files: announcer lines and music |
| `eva` | `request`, `queued`, `dropped`, `cleared`, `cut`, `start`, `skipped`, `end`, `stop-all` | The announcer: each line asked for, its priority and type, why it was dropped (`playing`, `waiting`, `standard-held`, `replaced`, `queue-full`), which line cut which, and when each starts and ends |
| `music` | `start`, `failed`, `stop` | Scores started and stopped, with file, level, fade and loop |
| `capture` | `start`, `stop`, `pause`, `resume` | The recording: mix rate, channels and frame length |

## Checking a recording

```
python scripts/audio_check.py run build/agent29-autotest-user/AudioTest/audio-eva-burst.wav
```

The tool reads `<name>.log` and `<name>.sounds\` next to the recording. It needs Python 3 and works best with
numpy; without numpy it matches at a lower rate and says so. `--json`, `--report` and `--spectrogram file.png`
write the findings as JSON, the text report to a file, and a greyscale spectrogram with each logged sound marked.

For each sound the log says started, the tool builds what the mixer should have added: the sound's files at its
pitch, its loops, its level and pan, each level change, and cut where the log says it stopped. It fits all of them at
their logged places at once, then judges each one. Part of a report:

```
  time s  frame kind  name                           file              expect  score  offset    level played  status
   1.644     83 eva   EVA_ChronosphereReady          ceva009              0.0   0.95    +0ms   -0.5dB   100%  found
   3.400    162 eva   EVA_WeatherDeviceReady         ceva010              0.0   0.99    +0ms   -0.1dB   100%  found

Flags:
  eva-late       EVA_ChronosphereReady starts 977 ms after the announcer was free for it (threshold 600 ms)
```

- `expect` is the level the log predicts, in dB. `level` is how far the recording is from it; within 6 dB counts.
- `score` is the correlation with the expected sound at its place (1 is a perfect match).
- `offset` is where the sound was heard relative to its logged start.
- `played` is how much of it is in the recording, in quarters.

| Status | Meaning |
| --- | --- |
| `found` | At its logged place and level, score at least 0.6 |
| `found-weak` | At its place and level, but other sounds blur the match |
| `masked` | Far quieter than what else plays there, so the tool cannot tell |
| `silent` | Silence where it should play |
| `not-found` | Something else plays there |
| `rate-mismatch` | Matches only played faster or slower |
| `already-playing` | Started before the recording, not checked |
| `too-short` | Cut after less than the tool needs to match |
| `no-file` | Its decoded file is missing from the sounds folders |

| Flag | Raised when |
| --- | --- |
| `missing` | A sound louder than `--quiet-db` (-45 dB) is `silent` or `not-found` |
| `late` | A sound is heard more than `--late-ms` (100 ms) from its logged start |
| `cut-short` | A sound the log says finished stops early in the recording |
| `pitch` | A sound matches only at another rate |
| `clipping` | Samples pass 0.9, where the mixer starts to shape peaks |
| `dc-offset` | A channel's mean is further than 0.005 from zero |
| `unexplained` | Sound louder than `--floor-db` (-50 dB) where no logged sound plays |
| `eva-overlap` | Two announcer lines play at once |
| `eva-repeat` | One announcer line plays again within `--repeat-seconds` (5 s) |
| `eva-late` | A line starts more than `--eva-late-ms` (600 ms) after the announcer was free for it: after it was asked for, and after the line before it ended |
| `capture-paused`, `rate-mismatch`, `no-numpy` | The recording paused, its rate differs from the log's, or numpy is missing |

### What the check cannot tell

- A sound far quieter than others playing at the same time is `masked`, not checked.
- Short sounds on top of loud ones match weakly; a `found-weak` sound is in the mix, but its level is less certain.
- The check compares the recording with the log, so a sound the game never asked for is absent from both and is not
  flagged. Test scripts pin the requests themselves through the `AUDIOLOG` lines in their expected logs.

## Finding sounds in a play-test clip

The `clip` mode needs no log. It looks for a library of known sounds in any recording, and reads video and compressed
audio through `ffmpeg` when it is on the PATH.

1. Write the library from a test run: `audiodump eva *` writes every announcer line in the current voice,
   `audiodump sound <Name>` or `audiodump sound *` the sound list's sounds, and `audiodump file <file>` a stream
   such as a score. They go to `AudioTest\library\`.
2. Run the tool on the clip:

   ```
   python scripts/audio_check.py clip clip.mp4 --library build/agent29-autotest-user/AudioTest/library
   ```

The report lists each known sound found, the time it occurs and its match score (0.6 or more by default,
`--score`). Sounds whose file name is a letter and `eva` and a digit, such as `ceva009`, count as announcer lines
(`--eva-pattern`), so `eva-overlap` and `eva-repeat` apply to them. The clip's own mix and compression lower the scores; a short sound under music and battle noise
may be missed.

## Announcer rules compared with gamemd

The announcer queue in `code/vox.cpp` and `code/voxqueue.cpp` differs from gamemd's `VoxClass` (decompiled
`gamemd.exe`) in these ways. The test scripts in the last column show our side in the log. We have not changed our
rules yet.

gamemd keeps four kinds of waiting line: a list of QUEUED_INTERRUPT lines, a list of other CRITICAL lines, one held
STANDARD line, and one QUEUE list per priority. It tries to start a line every update (about 30 ms) and whenever a
line is asked for: the first QUEUED_INTERRUPT line, else the first CRITICAL line, else the held line, else the first
line of the highest-priority QUEUE list. A line starts only when the announcer stream is idle and at least 500 ms have
passed since the last line started.

| Difference | gamemd | Ours | Certainty | Script |
| --- | --- | --- | --- | --- |
| First line of a burst | Starts at once (`FUN_00752760`, called from `VoxClass::PlayIndex`, 0x752480) | Waits a one-second settle | Certain | `audio-eva-queue`, `audio-eva-three` |
| Gap between lines | Next line may start once the stream is idle and 500 ms after the last one started (`DAT_00b1d4d0`, set after each start) | 500 ms after the last one ended | Likely; the idle test is `0x408070` | `audio-eva-queue` |
| Waiting-line limit | None; lists grow (`FUN_00407420` appends) | Eight; a ninth pushes out the lowest-ranked line, the oldest among equals | Certain | `audio-eva-burst` |
| Same line asked for again while waiting | Dropped only when its type matches the waiting copy (`FUN_00752680`) | Always dropped | Certain | `audio-eva-three` |
| STANDARD line | Held only when no QUEUED_INTERRUPT or CRITICAL line waits and the held line has lower priority; a held line is discarded when one of those starts (`FUN_00752590`, 0x752590) | Kept with lower rank than every QUEUE line | Certain | `audio-eva-three` |
| INTERRUPT line with nothing playing | Queued like a STANDARD or CRITICAL line; nothing is cleared | Clears every waiting line | Certain | none yet |
| Order of waiting lines | QUEUED_INTERRUPT list, CRITICAL list, held STANDARD, then QUEUE lists by priority, each first in first out | By control (CRITICAL, then QUEUED_INTERRUPT and INTERRUPT, then QUEUE, then STANDARD), then priority, then age; a CRITICAL QUEUE line goes first, where gamemd puts it in the top QUEUE list after the held line | Certain | `audio-eva-queue`, `audio-eva-interrupt` |
| Line asked for "at once" | No such flag; callers can only override the type | Treated as QUEUED_INTERRUPT and skips the settle | Uncertain; callers not all checked | none |
| Speech volume 0 | `PlayIndex` queues regardless | Dropped | Certain for `PlayIndex`; whether another flag stops speech is uncertain | none |
| Block and pause counters | The radar coming online and the chronoshift screen effect block new lines (`0x753570`/`0x753580`); movies and focus loss pause starting (`0x7535b0`/`0x753620`) | No equivalent | Likely to change radar and chronoshift announcements | none |
| Missing sample | The line is dropped; the next starts on the next update | The next line is tried at once | Certain; one update, so negligible | none |
| Per-line `Volume=` | Read from EVAMD.INI; whether it scales playback is not established | Scales the line | Uncertain | none |

INTERRUPT with a line playing matches: both stop the line, clear every waiting line and start the new one with no
gap (`PlayIndex` stops the stream through `0x407f40` and empties the lists through `0x752370`).

## Test scripts

| Script | Pins |
| --- | --- |
| `audio-unit-voice.txt` | A soldier's select reply plays in the unit voice group, centred |
| `audio-weapon-sound.txt` | A weapon sound is full level in view, quieter and panned off the side, silent out of range |
| `audio-eva-three.txt` | Three STANDARD lines in one frame: only the highest-priority one plays |
| `audio-eva-queue.txt` | Three QUEUE lines play one at a time, highest priority first |
| `audio-eva-burst.txt` | Ten QUEUE lines: the two oldest are pushed out |
| `audio-eva-interrupt.txt` | An INTERRUPT line cuts the line playing and clears the queue; uses the test mod in `audio-eva-rules\` |
| `audio-music-start.txt` | A score opens a music stream; stopping it ends the stream and the playlist starts the next |

The tool's own tests, on synthetic recordings, are in `tests/audiocheck/` and run with the unit tests.
