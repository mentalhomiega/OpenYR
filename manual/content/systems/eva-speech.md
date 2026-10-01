---
title: EVA speech
summary: The announcer's lines come from EVAMD.INI in the voice of the player's side, wait in a queue of up to eight, and play one at a time.
category: audio-speech
keys: [VoiceVolume]
---

Every line spoken during a game goes through one queue and plays one at a time. That includes the announcer's lines and the lines that [Play speech](/mapping/actions/taction-play-speech/) asks for. Up to eight lines can wait in the queue.

## Where the lines come from

The announcer's lines are listed in [EVAMD.INI](/formats/eva-ini/), which names each line's sample for the Allied, Russian and Yuri voices and sets how the line waits. The voice follows the player's side. The game asks for each announcement by its name in that file, such as `EVA_ConstructionComplete`.

Some announcements the game makes have no line in Yuri's Revenge, such as Tiberian Sun's "silos needed" and "cloaked unit detected", and say nothing.

Without EVAMD.INI, the game speaks Tiberian Sun's lines instead, each from the `.AUD` file of its own name.

A line whose file cannot be opened is skipped, and the next waiting line plays in its place.

## Timing

When a line is requested while nothing is speaking or waiting, the game waits about a second before playing it. The other lines an event requests over the next few frames join the queue during that second. After each line ends, the next one waits half a second.

A line can also be requested to play at once. It ends the one-second wait and goes ahead of the ordinary waiting lines, but it does not cut a line that is already speaking, and it still waits out the half-second gap. The incoming-transmission call that opens a radar movie is requested this way.

## Order

Each line has a `Type` and a `Priority` in EVAMD.INI. Waiting lines play in this order:

1. `Priority=CRITICAL` lines.
2. Lines requested to play at once, and `INTERRUPT` and `QUEUED_INTERRUPT` lines.
3. `QUEUE` lines.
4. The one waiting `STANDARD` line.

Within each group, a higher priority plays first, and lines of equal priority play oldest first.

- An `INTERRUPT` line empties the queue and cuts the line that is speaking.
- Only one `STANDARD` line waits at a time. A new one replaces it only with a higher priority; otherwise the new line is dropped.

A line that is speaking or already waiting is not added again. When all eight places are taken, the new line takes the place of a waiting one: a `STANDARD` line first, then the lowest-priority `QUEUE` line, then the lowest-priority line of the second group, with the oldest going first among equals. A critical line is replaced only when every waiting line is critical.

Without EVAMD.INI, mission accomplished and mission failed are the critical lines, and every other line is a `QUEUE` line of the same priority.

## What stops a line

Stopping speech empties the queue and cuts the line that is speaking. It happens when:

- a mission is lost, once every waiting line has played or about five seconds pass;
- a scenario or saved game loads, because the game switches voices.

Aborting a game stops speech at once. The announcer then announces the exit, and speech stops again when that line ends or after about five seconds. While EVA is turned off, no exit announcement plays.

## Turning EVA off

An EVA line is one whose name in EVAMD.INI starts with `EVA_`, or without that file, one whose file name starts with `00-` or `01-`. [Disable Speech](/mapping/actions/taction-disable-speech/) keeps new EVA lines out of the queue until [Enable Speech](/mapping/actions/taction-enable-speech/) lets them in again. Other lines, such as a mission's dialogue, are still queued, and lines already waiting or speaking still play.

Each scenario starts with EVA enabled. A saved game restores the setting it had when it was saved.

## Volume

[`VoiceVolume`](/keys/voicevolume/) sets the volume of all speech, and a line's `Volume` in EVAMD.INI scales its own. A change to `VoiceVolume` applies at once, including to the line that is speaking.

While any of these holds, every request to speak is refused and nothing is queued:

- the game was started with the [quiet launch option](/using/command-line/quiet/);
- `VoiceVolume` is below 1/255 (about 0.004), which includes zero;
- no audio device is available.
