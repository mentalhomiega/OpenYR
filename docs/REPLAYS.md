# Replays

A replay is a recording of a skirmish that the game plays back to the same result. The recording holds the events the match executed, not the video, so a replay is small and plays at the speed of the simulation. Playback proves it reached the same result by comparing hashes of the game state against the ones the recording kept.

## What is recorded

Recording is off by default. `RecordReplays=yes` under `[Options]` in `RA2MD.INI` records every skirmish that a launch file started (`-SPAWN`). A skirmish started from the menus, a campaign mission, a network game and a resumed save are not recorded. The file is written to `Replays\<map>-<date>-<time>.rep` in the user directory.

An autotest script can name the file with `recordreplay <name>`, which also turns recording on.

## The file

`code/replayfile.h` owns the layout. The file holds, in order:

| Part | Content |
| --- | --- |
| Magic and revision | `OYRREPLY` and the format revision, 1 |
| Build | Packed version, commit, whether the tree had uncommitted changes, architecture |
| Data | Map file name and checksum, checksums of the rules, art and AI files in force, the mod signature |
| Match | Random seed, game speed setting, frames between hashes, the house the player controlled |
| Launch file | The text of the `SPAWN.INI` that started the match |
| Records | One `E` record for each executed event, one `H` record for each state hash, and a closing `Z` record |

All numbers are little-endian. An event record holds the frame it ran on, its type, its own frame, the house that issued it and its payload. The payload is the event's wire form, `EventClass::EventLength` bytes of `Data`, or the text of a beacon message. A hash record holds the frame and two 32-bit words from `Compute_Sim_State_Hash` in `code/statehash.h`: one over the position, facing, strength, mission, limbo state and owner of every object, each house's credits and the random-number state, and one over every object's target. Neither covers selection or other view state.

A hash is recorded every 30 frames. The closing record names the last frame that ran and its hash. A recording that ended with a crash has no closing record and plays as far as it goes.

The recording does not use the saved game format or the network protocol. Any change to a field of the file bumps `ReplayFile::REVISION`, and a build refuses a replay of another revision.

## Playback

`-REPLAY=<file>` plays a replay, looked up in the `Replays` folder when the path names no folder. An autotest script does the same with `playreplay <name>`. Both start the match from the launch file text in the replay, not from `SPAWN.INI`.

The game refuses the replay, writes a `REPLAY refused:` line to the debug log and ends, when the build (version and commit), the map file, the rules, art or AI checksums, the mods, the random seed or the player's house differ from the recording. Run with the same `-MOD=` switches as the recording.

Each frame, playback discards the orders the player and the autotest script queued, and queues the recorded events for that frame in their place. `Replay_Frame_Begin` in `code/replay.cpp` does this in `Queue_AI_Normal`, so the events run through `Execute_DoList` as they did in the recording. Nothing else is replaced: the view, the selection and the menus still respond, and a script step that changes the simulation without an event (the `strike`, `move` and `attack` steps in a skirmish) changes the match.

Each recorded hash is compared on its frame. The debug log gets `REPLAY mismatch frame N:` for the first three differences and one `REPLAY result:` line at the end, which says `MATCH` or `MISMATCH` with the number of frames, events and hashes checked and the first frame that differed. An unattended run exits with 0 on a match, 1 on a mismatch and 2 when the replay is refused. A playback outside an unattended run carries on after the last recorded frame with no events.

## Testing

`tests/replay/record-skirmish.txt` plays a short skirmish on `XMP29U2.MAP` with orders that go through the event queue, and `tests/replay/playback-skirmish.txt` plays the result back. Run them one after the other with `scripts/run_autotest.ps1` in the same build, as the headers of the scripts show. `tests/replayfile` is a contract test of the file format.

A replay stays in step only while the simulation depends on the recorded events alone. A simulation that depends on the pace of the game, such as the frame rate or the wall clock, plays back differently from the recording. Record and play back at the same game speed to avoid that.
