---
title: EVA speech
summary: The announcer's lines come from EVAMD.INI in the voice of the player's side, wait by their Type and priority, and play one at a time.
category: audio-speech
keys: [VoiceVolume]
---

Every line spoken during a game goes through one queue and plays one at a time. That includes the announcer's lines and the lines that [Play speech](/mapping/actions/taction-play-speech/) asks for. The queue has no limit.

## Where the lines come from

The announcer's lines are listed in [EVAMD.INI](/formats/eva-ini/), which names each line's sample for the Allied, Russian and Yuri voices, and for each voice that `[EVATypes]` adds, and sets how the line waits. The voice follows the player's side, and a side's [`EVA.Tag`](/keys/eva.tag/) can name another. The game asks for each announcement by its name in that file, such as `EVA_ConstructionComplete`.

Some announcements the game makes have no line in Yuri's Revenge, such as Tiberian Sun's "silos needed" and "cloaked unit detected", and say nothing.

Without EVAMD.INI, the game speaks Tiberian Sun's lines instead, each from the `.AUD` file of its own name.

A line whose file cannot be opened is skipped, and the next waiting line plays in its place.

## Timing

A line starts at once when nothing is speaking and at least half a second has passed since the last line ended. Otherwise it waits, and the next waiting line starts half a second after the line speaking ends.

## Order

Each line has a `Type` and a `Priority` in EVAMD.INI. A line waits in one of four places:

1. A `QUEUED_INTERRUPT` line joins the interrupt list.
2. A `QUEUE` line joins the list of its priority. A `QUEUE` line of `CRITICAL` priority waits here, not with the critical lines.
3. Any other line of `CRITICAL` priority joins the critical list.
4. Any other line becomes the one held `STANDARD` line, if no interrupt or critical line waits and the held line, if any, has a lower priority; it then replaces the held line. Otherwise the new line is dropped.

The next line to play is the first of the interrupt list, else the first of the critical list, else the held line, else the first line of the highest-priority `QUEUE` list. Each list plays oldest first. When an interrupt or critical line starts, the held line is dropped.

- An `INTERRUPT` line asked for while a line speaks cuts that line, empties the queue and starts at once. Asked for while nothing speaks, it empties nothing and waits as a `STANDARD` line.
- [Play speech](/mapping/actions/taction-play-speech/) asks for its line as an `INTERRUPT` line, whatever its `Type`.
- The incoming-transmission call that opens a radar movie is asked for as a `STANDARD` line of `CRITICAL` priority.

A line asked for while it speaks is not added again. A line asked for while it waits is dropped when it was asked for with the same `Type` as the waiting copy; with another `Type`, a second copy waits.

Without EVAMD.INI, mission accomplished and mission failed are `STANDARD` lines of `CRITICAL` priority, and every other line is a `QUEUE` line of the same priority.

## What stops a line

Stopping speech empties the queue and cuts the line that is speaking. It happens when:

- a mission is lost, once every waiting line has played or about five seconds pass;
- a scenario or saved game loads, because the game switches voices.

While the chronoshift screen effect runs, and while a radar movie's incoming-transmission call speaks, every new line is dropped. While a movie plays, and while the game window does not have the focus, the line speaking is paused and no line starts; requested lines still wait.

Aborting a game stops speech at once. The announcer then announces the exit, and speech stops again when that line ends or after about five seconds. While EVA is turned off, no exit announcement plays.

## Turning EVA off

An EVA line is one whose name in EVAMD.INI starts with `EVA_`, or without that file, one whose file name starts with `00-` or `01-`. [Disable Speech](/mapping/actions/taction-disable-speech/) keeps new EVA lines out of the queue until [Enable Speech](/mapping/actions/taction-enable-speech/) lets them in again. Other lines, such as a mission's dialogue, are still queued, and lines already waiting or speaking still play.

Each scenario starts with EVA enabled. A saved game restores the setting it had when it was saved.

## Volume

[`VoiceVolume`](/keys/voicevolume/) sets the volume of all speech: EVA lines, taunts and the lines that Play speech asks for. A line's `Volume` in EVAMD.INI scales its own. A change to `VoiceVolume` applies at once, including to the line that is speaking. Unit and building replies are not speech and follow [`UnitVoiceVolume`](/keys/unitvoicevolume/).

A line asked for at a `VoiceVolume` of zero still takes its turn, without sound, so the lines after it wait as they would at any volume.

While either of these holds, every request to speak is refused and nothing is queued:

- the game was started with the [quiet launch option](/using/command-line/quiet/);
- no audio device is available.
