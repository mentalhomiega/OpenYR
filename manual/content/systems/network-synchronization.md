---
title: Network synchronization
summary: How a network game adjusts its command delay and packet rate to the players' measured round trips and process times.
category: multiplayer-networking
keys: []
---

A network game tags every command with the frame on which every machine executes it. Two settings control that schedule. The send period is how many frames go into one packet, so each machine sends once per period. The look-ahead is how many frames ahead a new command is scheduled, which gives it time to reach the other players. The pair is written as send period/look-ahead: `2/6` means a packet every two frames and commands scheduled six frames ahead.

[Network packet validation](/systems/network-packet-validation/) owns what a packet must satisfy. [Network transport timing](/systems/network-transport-timing/) owns how each link measures its round trip and retries lost packets.

## Timing policy

By default, online and LAN games use gamemd's timing. The game speed sets the frame rate and the starting look-ahead. The master then sets the look-ahead again every 256 frames, from the worst round trip and the frame rate, and sends each change as a timing event.

At the start, the send period and the look-ahead come from the game speed:

| Game speed | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Frame rate | 60 | 45 | 30 | 20 | 15 | 12 | 10 |
| Send period | 10 | 10 | 5 | 5 | 5 | 5 | 5 |
| Look-ahead | 40 | 40 | 30 | 20 | 20 | 20 | 10 |

A game through a tunnel adds 20 frames to the look-ahead.

From frame 256 on, the look-ahead is the round trip, in sixtieths of a second, times 1.5, times the frame rate, divided by 120, plus 10 frames. The result is rounded up to a whole number of send periods. The send period is 10 at 31 frames per second and up, and 5 otherwise. The look-ahead is at least three send periods and at most 250 frames. A rise is limited to one send period per evaluation. A fall waits as described under [Applying a change](#applying-a-change). A round trip of 100 ms at 60 frames per second gives a look-ahead of 30 frames.

Under the default timing, each frame lasts 1000 divided by the frame rate in whole milliseconds, rounded down. An online game also adds 10 ms to a frame for each quarter, half and three quarters of the look-ahead that the slowest peer lags behind. This starts at a look-ahead of 31.

The [adaptive table](#adaptive-policy) is the other option.

## Adaptive policy

This section describes the adaptive table. It runs only when the [launch file](/formats/spawn-ini/#a-game-against-other-machines) sets `TimingPolicy=Adaptive`. Under the adaptive table, a compressed game packs a run of frames into one packet. It starts at `2/6` and then adjusts both settings during play. Every player reports its measurements, and one player, the [master](/systems/out-of-sync-recovery/#the-master), evaluates the reports and sends each new timing to everyone.

### Timing rungs

The timing moves in steps called rungs, numbered 1 to 10. Rung N sends every N frames and schedules commands 3N frames ahead, so rung 3 is `3/9`. Rung 1 is the exception, at `1/4`.

The master picks the lowest rung whose look-ahead covers a one-way trip plus one send period. The one-way trip is half the worst reported round trip, converted to frames at the game's [frame rate](#frame-rate) and rounded up. Rung 1 covers a one-way trip of up to 3 frames, and every higher rung N covers up to 2N frames.

A one-way trip longer than 20 frames needs more than rung 10. The send period then stays at 10 frames, and the look-ahead grows in steps of 10 frames to cover the trip, up to 250 frames.

For example, at 30 frames per second a 200 ms round trip is a one-way trip of 3 frames, which rung 1 covers. With a quarter added for headroom (see [Choosing a target](#choosing-a-target)), the same link counts as 250 ms. That is a one-way trip of 4 frames, which needs rung 2.

### Reports

Each player sends a report 32 and 64 frames after the game starts, then every 128 frames. A report carries:

- the player's average processing time per frame;
- the longest time it waited for the other players during its last two report intervals;
- its worst round trip to any other player.

A player leaves the round trip out while any of its links lacks a [clean measurement](/systems/network-transport-timing/). A link that is retransmitting keeps reporting its last measured round trip.

Reports travel as network events, so every machine keeps the same record of them. A report expires 512 frames after it executes. The record starts from the players seated when the game begins, and it drops a player's report when the event that removes that player executes.

A player that has not reported any round trip 512 frames after the record started forces the widest timing, `10/250`. Any other player whose latest report has expired or lacks a round trip holds the timing where it is. Fresh round trips from the other players can still worsen it.

### Choosing a target

The master evaluates the reports 64 and 128 frames after the game starts, then every 256 frames.

The headroom estimate is the worst reported round trip raised by a quarter.

If every player has a fresh report with a round trip at the 64- or 128-frame evaluation, the master sets the target from the headroom estimate. If the reports are still incomplete at 128 frames, the target falls back to `3/9`.

After that, the master compares each evaluation's result with the current target, which is the last timing it chose:

- **Worse:** if the worst round trip, without headroom, needs a higher rung, or a longer look-ahead on the same rung, the master sends that timing at that evaluation. It applies as described under [Applying a change](#applying-a-change).
- **Better:** an evaluation counts toward an improvement only when all of the following hold:
  1. the worst round trip, without headroom, needs a lower rung, or a shorter look-ahead on the same rung;
  2. no player reported a wait of 0.1 s or longer;
  3. at least 256 frames have passed since the master last changed the timing, unless it has never changed it or a descent is already running;
  4. the headroom estimate also needs a lower rung, or a shorter look-ahead on the same rung.

The first improvement comes after three such evaluations in a row. It moves one rung down. At rung 10 with an extended look-ahead, when the worst round trip without headroom still needs rung 10, the improvement cuts the look-ahead straight to what that round trip needs.

That improvement starts a descent. While it runs, each following evaluation that meets all four conditions moves down one more rung. The descent and the count of three start over after an evaluation that fails a condition, after a worsening, and after an evaluation that finds a report missing.

### Applying a change

A timing change arrives as an event. A change that shortens neither the send period nor the look-ahead applies when that event executes.

A change that shortens either setting waits until every command scheduled under the old look-ahead has come due. It then activates on the first frame that is a multiple of both the old and the new send period and is not before that point. On that frame:

1. the new send period applies;
2. the look-ahead drops by one old send period, but not below the target's, then rounds up to a whole number of new send periods;
3. each following send frame removes one more new send period from the look-ahead until it reaches the target.

A change that lengthens the send period but shortens the look-ahead switches the send period when its event executes. The look-ahead then steps down to the target in the same way.

A command scheduled for a frame that is no longer a send frame executes on the next send frame, on every machine alike. A recording stores it with that send frame's commands, still tagged with its original frame.

If the master picks another target before a staged decrease finishes, the new target replaces it. It is applied or staged from the timing in force at that moment, by the rules above.

### Frame rate

Each timing event also carries the frame rate every machine runs at. The master sets it to the lower of the rate the game speed allows and the rate the slowest player's processing time allows. While any player lacks a fresh report, the master keeps the current frame rate. Each machine paces its frames to this rate. Under the default timing, a frame waits 1000 divided by the rate in whole milliseconds, plus the pacing waits in [Timing policy](#timing-policy). The rate applies when the timing event executes. For a decrease, it applies when the decrease activates.

### Starting master

At the start, the master is the host that the host's announcement names. A machine that has not yet received the announcement uses the lowest seat and switches to the host when the announcement arrives. The announcement asks for an acknowledgement, so a lost copy is sent again.

### Master handoff

Only an executed removal event changes the master. A machine that loses its connection to the master keeps it as master until then. When the master is removed, the first remaining human player in house order takes over. The new master keeps the current target. Any running descent or count of three starts over, and the new master waits 256 frames before an improvement.

## Player feedback

When the options dialog opens in an internet game, a disabled Connection slider shows the rung that the adaptive table would choose for the worst measured round trip and the desired frame rate. The slider reports the link. It does not show the timing in force, which under the default timing is not from that table. The slider is mirrored, so rung 1 sits at its right end. The label beside it names the tier and the rung, such as `Fast (rung 2)`.

| Rungs | Tier |
| --- | --- |
| 1 and 2 | Fast |
| 3 to 5 | Normal |
| 6 to 8 | Poor |
| 9 and 10 | Bad |

A look-ahead longer than its rung normally uses also reads as Bad. This happens at rung 10 with an extended look-ahead, and briefly while a decrease from it steps the look-ahead down.

With the adaptive table, when a timing event executes with a target in a different tier from the previous target, the message list shows "Connection quality target:" followed by the new tier. For a decrease, the message appears before the staged change takes effect.

The Speed slider in the same dialog sets the game speed, which caps the [frame rate](#frame-rate).

## Compatibility

`NETWORK_REPORT` is a network event that OpenTS adds, and it appears in multiplayer recordings. Every player therefore needs the same OpenTS snapshot, and a recording should be played by the snapshot that wrote it. The events that Tiberian Sun already had keep their IDs.

The timing reads the measured round trips directly. `LATENCYFUDGE` keeps its event ID and its session setting so that recordings still play, but nothing sends it, and the timing does not read it.
