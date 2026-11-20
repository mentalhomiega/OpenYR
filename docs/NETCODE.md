# Network timing and order delay

An order issued on frame F executes on frame E = B + MA. MA is the look-ahead in frames, and B is the first send frame at or after F. A send frame is a multiple of the send period, FSR. A master machine sets FSR and MA with a timing event, and every machine applies the event on the same frame. All machines therefore execute each order on the same frame.

Two policies pick FSR and MA:

- **gamemd timing** is the default. It follows gamemd's online timing. The game speed sets the start values, and the master re-sets the look-ahead every 256 frames from the worst round trip. Online games (the spawner's game mode 4 here, `Session.Type == GAME_INTERNET`) and LAN games (mode 3, `GAME_IPX`) both use it.
- **The adaptive table** is OpenYR's own policy. It is selected with `TimingPolicy=Adaptive` in a spawner launch file. See [Adaptive policy](#adaptive-policy).

Under gamemd timing, a match from the built-in CnCNet lobby whose host chose a send period and no look-ahead takes its look-ahead from the measured round trip instead of gamemd's formula. See [Measured look-ahead](#measured-look-ahead).

The timing event is the same for both. Only the master evaluates, so a machine's own choice of policy decides the events only while that machine is the master.

This page covers protocol 2 (`COMM_PROTOCOL_MULTI_E_COMP`). Every assignment to `Session.CommProtocol` in this build uses it, so the protocol 0 and 1 branches do not run in a network game.

## Master

The master is the host's seat once the host announces itself. Before the first frame, the announced seat replaces the lowest seat that connection setup chose, so every machine names the same master. Only the machine that names itself master sends timing events. If the machines name different masters, none sends them and the rate stays at its start value.

## Order delay

An order issued on frame F executes on frame E = B + MA, where B is the first send frame at or after F. The delay E − F is MA plus between 0 and FSR − 1 frames. MA is a multiple of FSR in every valid setting, so E is a send frame too.

- The order enters `OutList` with the frame it was created in. `EventClass` constructors set `Frame` to `::Frame`.
- Every frame runs the network step. It returns early on frames that are not send frames (`Process_Send_Period`). Game logic runs on every frame.
- On a send frame, `Add_Compressed_Events` sets each queued order's frame to E, moves it to `DoList`, and sends it.
- Each machine runs `Execute_DoList` for E.

An order created before the network step of frame F can go out in frame F. The 120-second run below shows this: an order created on frame 120 went out on frame 120.

### Frame advance

A machine moves past a send frame when both of these hold (`Can_Advance`):

- Its frame is below the oldest peer frame plus MA. A peer's frame is its `FRAMEINFO` frame minus its `Delay` field, which is the peer's frame at its last send.
- It has received every order that each peer reports as sent.

When either check fails, `Wait_For_Players` services the connections and resends the frame-sync packet about once a second (`TIMER_SECOND`, 60 ticks). It also draws the stall marker. If the wait passes `ReconnectTimeout`, `Handle_Timeout` runs. If that does not clear the stall, the game reports the player as not responding.

## gamemd timing

We read these functions in the decompiled `gamemd.exe` (YR 1.001) in `decomp/`:

- `Queue_AI_Multiplayer` (`_global.c`, from line 13451): the master's evaluation, the TIMING event, the response-time term.
- `Game::MainLoop` (`Game.c`, from line 591): the frame wait and the online pacing waits.
- `CChatEventSink::OnGameStart` (`CChatEventSink.c`, from line 3): the start values and the start ping.
- `EventClass` TIMING handler (`EventClass.c`, line 907): how a machine applies an event.

The menu sets game mode 3 for the UDP network game and mode 4 for the online game (`_global.c` 6398 and 6682). Modes 0 and 5 do not run this timing.

### Start values

When a game starts, gamemd sets the send period and look-ahead from the game speed (`OnGameStart`). The frame rate starts at the rate for the speed.

| `GameSpeed` | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Frame rate | 60 | 45 | 30 | 20 | 15 | 12 | 10 |
| Send period | 10 | 10 | 5 | 5 | 5 | 5 | 5 |
| Look-ahead | 40 | 40 | 30 | 20 | 20 | 20 | 10 |

A match through a tunnel adds 20 frames to the look-ahead. gamemd tests `unknown_2854[0x20f]` for this, and that flag comes from the lobby data. We take the spawner's `[Tunnel]` section as the same condition, which is an inference.

gamemd also sends the start frame rate and look-ahead in a precalculated timing event at frame 0. The decompile does not show that path's look-ahead in every case, so we set the table's look-ahead on each machine at start and send no start event.

Each machine sets these values before its first frame, from `GameSpeed` and the tunnel flag. A launch file refuses a `GameSpeed` outside 0 to 6, so the table covers every game the spawner starts.

### Evaluation

The master evaluates when the frame is a multiple of 256 (`(char)CurrentFrame == 0`). Frame 0 is not an evaluation, because no player has reported a process time yet. gamemd makes no evaluation while any seated player's process time is missing. We also make none while a report is older than 512 frames, where gamemd keeps the last value. The rules that follow are the ones an evaluation applies.

1. **Frame rate.** F is the lower of the speed's rate and 1000 divided by the worst process time among seated players, in milliseconds, rounded down. A worst process time of 0 gives 60. Each player's process time is the average from its last report.
2. **Send period.** FSR is 10 when F is 31 or more, and 5 otherwise (`_global.c` 13766 to 13770). gamemd applies this to online games. For LAN games it keeps the send period it started with, which is the same at speeds 0 to 2. We apply the online rule to both (see open questions).
3. **Response time.** R is the worst connection's average response time, in ticks of 1/60 second (`IPXManagerClass::ResponseTime`, `IPXManagerClass.c` 585). We convert each player's smoothed round trip to ticks: R = floor(round trip in ms × 60 / 1000).
4. **Latency stage.** The stage is 1. gamemd sets 1 for a start ping up to 1000 ms and 2 above that, and the lobby's latency setting can change it. We always use 1.
5. **Scaled response and add-on.** With stage 1, R' = R + R/2 and the add-on is 10 frames. Stage 2 gives R' = 2R and 20 frames, and stage 3 gives R' = 3R and 30 frames. When R is 0, nothing is scaled and the add-on is 0.
6. **Target.** T = floor(R' × F / 120) + add-on. This is the one-way trip in frames plus the add-on.
7. **Rise limit.** If T is above the current look-ahead and at least one send period above it, T is the current look-ahead plus FSR. A fall is not limited.
8. **Alignment.** MA = ceil(T / FSR) × FSR.
9. **Floor.** MA is at least 3 × FSR.
10. **Ceiling.** MA is at most ceil(250 / FSR) × FSR. That is 250 at FSR 5 or 10.
11. **Event.** The master sends TIMING with F, MA and FSR. gamemd sends this at every evaluation. We send it only when the values differ from the current target. The values that every machine applies are the same either way.

Special flags: gamemd adds 10 to MA in the event when the scenario's special-flags bit 0x1000 is set, and the TIMING handler subtracts the same 10. OpenYR does not read that bit, so it never adds 10.

### Applying a timing event

The TIMING handler (`EventClass.c` 907) sets the frame rate, the look-ahead (less 10 when bit 0x1000 is set) and the send period when the event executes. A rise also sets a window end (`DAT_00a8b1dc`) from the event frame. We did not trace how the order code uses it. gamemd applies a fall at once. OpenYR stages a fall, as described under [Applying a change](#applying-a-change).

### Frame wait

`Game::MainLoop` arms each network frame for 1000 / F whole milliseconds, counted from the start of the frame (`DAT_00887330`, `DAT_00887328`). The fraction is dropped, so a frame at 45 FPS waits 22 ms (45.5 FPS at most) and a frame at 60 FPS waits 16 ms (62.5 FPS at most). Solo games use a different rule, with exact periods (see [Frame rate and game speed](#frame-rate-and-game-speed)).

In an online game (mode 4) with look-ahead MA of at least 31, each of floor(MA/4), floor(MA/2) and floor(3MA/4) adds 10 ms to the frame's wait when the largest per-connection value is at least that many frames. The waits apply only when the largest value is nonzero. The table holding the per-connection values has no writer that we identified, so OpenYR uses the frames the most-behind connected peer trails this machine (see open questions).

### Tunnelled response term (not reproduced)

In an online game through the packet router (`unknown_2854[0x20f]`), gamemd sets `unknown_2854[0x45]` (`SessionClass.c` 327). With that flag, R is the global connection's average response plus the largest per-connection value, which defaults to 20 ticks (`_global.c` 13766 to 13771). We do not reproduce this term. A tunnelled match here therefore keeps a shorter look-ahead than gamemd would choose in steady state.

## Adaptive policy

This section describes the adaptive table. It runs only when the spawner's launch file sets `TimingPolicy=Adaptive`. The default policy is [gamemd timing](#gamemd-timing). The master, the reports and the events below apply to both policies, except where a section says otherwise.

### Rungs

The policy chooses a rung from 1 to 10. Rung r sets FSR r and MA 3r. Rung 1 sets FSR 1 and MA 4.
Every game starts on rung 2 (FSR 2, MA 6). MA never exceeds 250.

### Choice

The master computes the desired frame rate F (see [Frame rate](#frame-rate-and-game-speed)) and R,
the worst smoothed round trip among seated players, in milliseconds. The one-way delay is
ceil(R × F / 2000) frames. A rung must hold that one-way delay plus one send period, and MA must be
at least three send periods. The limits follow:

- Rung 1 covers R ≤ 6000 / F ms.
- Rung r ≥ 2 covers R ≤ 4000 × r / F ms.

The policy picks the lowest rung whose limit covers R.

The drop check uses 1.25 × R, rounded up, in place of R. A link must therefore improve about 20%
past a limit before the rung drops. At 45 fps, rung 1 replaces rung 2 once R is 106 ms or less.

### Reports

Each machine sends a network report every 128 frames, or every 32 frames for the first two. A report
holds the machine's average process time, its worst smoothed round trip over its connections, and
its longest wait. The master takes the worst value of each across seated players.

- A player with no round-trip sample makes the census incomplete.
- A player that has been seated for 512 frames without a round-trip sample forces the conservative
  choice: rung 10 with MA 250. At 43 fps, 250 frames is about 6 seconds.

### When the policy evaluates

- The first evaluation is at frame 64 if the census is complete. If it is still incomplete at frame
  128, the policy picks rung 3 (FSR 3, MA 9).
- After that it evaluates every 256 frames.
- A rise (a worse link) applies at the next evaluation.
- A drop needs three good evaluations in a row. While the drops keep qualifying, each following drop
  needs one good evaluation. The first evaluation that does not qualify ends this, and the next drop
  then needs three again. A drop also needs the last change to be at least 256 frames old, unless a
  drop is already under way.
- A census with a wait of 100 ms or more blocks a drop.
- Each drop step moves one rung.

At 43 fps, the first drop needs about three evaluations, roughly 18 seconds.

### Applying a change

The master sends the new FSR, MA and frame rate in a `TIMING` event. Each machine applies it on the
event's frame.

- A rise applies at once. Orders already scheduled inside the new window move to the window's end
  (`NewMaxAheadFrame1` and `NewMaxAheadFrame2`). This keeps a machine that runs ahead to the new
  horizon from passing an order it has not received.
- A drop is staged. It activates at the first multiple of lcm(old FSR, new FSR) at or after the
  event frame plus the old MA. At activation, MA becomes the larger of the new MA and (old MA − old
  FSR), aligned to the new FSR. Each later send frame lowers MA by the new FSR until it reaches the
  new value.

In the measured runs, the first drop (2/6 to 1/4) was sent on frame 70. It activated on frame 76,
and MA went from 6 to 4 at once.

## Frame rate and game speed

A campaign or skirmish takes its frame length from the game speed, as a network game does. By default both use the network table below, at every speed. Speed 0 is 60 frames a second, not unlimited, and a campaign has its speed slider. Fast-forward removes the limit.

| `GameSpeed` | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Default, campaign and skirmish frames a second | 60 | 45 | 30 | 20 | 15 | 12 | 10 |

The slider names the speeds from 6 to 0 Slowest, Slower, Slow, Medium, Fast, Faster and Fastest.

`ClassicGameSpeeds=yes` under `[Options]` in `RA2MD.INI` switches to gamemd's own solo pacing (the frame timer at `0x55E160`, started in `Game::MainLoop` at `0x55D340`). The timer counts units of 16 ms (`SystemTimer::GetTime` is `timeGetTime() >> 4`, `0x6C8C40`), and a skirmish frame lasts as many units as the game speed is. Speed 0 has no limit.

| `GameSpeed` | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Classic skirmish frame | no limit | 16 ms | 32 ms | 48 ms | 64 ms | 80 ms | 96 ms |
| Classic skirmish frames a second | no limit | 62.5 | 31.25 | 20.83 | 15.62 | 12.5 | 10.41 |

A classic campaign mission (`GAME_NORMAL`) ignores the game speed: gamemd sets the frame timer to 2 units whatever the option is and hides the slider, so every campaign frame lasts 32 ms. The in-game game controls dialog hides the slider in a classic campaign too. `NetTiming::Solo_Frame_Period` picks the period for both tables, and the pacer, the menu battle and the replay list's length read it. The menu battle runs at game speed 2, 30 frames a second (31.25 with the classic option). In either case the option itself still selects the animation delay table, which depends on the speed value and not on the frame rate, so a solo game and a network game at one speed delay their animations alike. A recording does not store which table it was played at, so the replay list works its lengths out with the option as it is now.

The option's switch is on the game controls dialog next to the speed slider, and the Settings screen's Game tab shows the same dialog. The classic style's main menu options page has no speed slider and no switch. A network game ignores the option.

A solo frame ends at a deadline on the steady clock in nanoseconds (`NetTiming::FrameDeadline`, given a `NetTiming::FramePeriod` so a period need not be a whole rate). Each deadline is the previous one plus the period, with the fractional part carried, so a minute holds exactly as many frames as the rate says. gamemd times each frame from its own start and never shortens a later frame to make up for a slow one; the pacer does the same. A frame that starts more than 2 ms after the deadline (`FRAME_LATE_SLACK_NANOSECONDS`), because of a slow frame, a pause, a dialog or a change of rate, restarts the schedule from the time it started. A slow frame therefore lowers the average rate and never raises the rate above the setting. The wait sleeps on a high-resolution timer and spins only for the last 1.5 ms (`Fine_Wait_Until` in `code/framewait.cpp`).

The map is drawn while a frame waits for its deadline. `RenderFrameRate`, `FrameRateLimit` and `VSync` set how often it is drawn and presented; they do not change when game frames end, except that a draw longer than the time left is skipped.

In a network game, the desired frame rate F is the lower of the game-speed rate and 1000 divided by the longest process time among seated players. Process time runs from the start of `Main_Loop` to the end of AI logic. It includes the map render and excludes the network wait. A process time of 0 ms gives 60 fps. While a seated player's process report is missing or stale, F keeps its synchronized value.

| `GameSpeed` | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Network frame rate | 60 | 45 | 30 | 20 | 15 | 12 | 10 |

Values above 6 give 60 fps here. gamemd gives 60 divided by the speed for speeds 2 and up. The launch file refuses a value above 6.

A network frame waits as described under [Frame wait](#frame-wait) (gamemd's rule). The adaptive policy uses the solo deadline above, which carries the fraction so the average rate is exact. It applies only with `TimingPolicy=Adaptive`.

MA counts frames, so its time depends on F. A look-ahead of 30 frames is 500 ms at 60 fps and 667 ms at 45 fps. A slow machine lowers F for every player, so the same look-ahead takes longer in milliseconds.

The game starts with the rate and look-ahead in [Start values](#start-values). Before the change, a network game started at 30 fps and waited for the first timing event.

## Host choice

The host can fix the send period (FSR), the look-ahead (MA) or both for a match. The built-in CnCNet lobby and the LAN lobby take the chat commands `/FrameSendRate`, `/MaxAhead` and `/ProtocolVersion` (alias `/Protocol`). A launch file carries the choice as `FrameSendRate`, `MaxAhead` and `Protocol`. Without a choice, nothing in this page changes. The code is `NetTiming::HostTimingChoice` and `NetTiming::HostNetworkOptions` in `code/nettiming.h`, kept in `Session.NetworkTimingChoice`.

- **Ranges.** FSR is 1 to 10. MA is 4 to 250. A value outside its range is refused by the command and passed over in a launch file. MA is rounded up to a whole number of send periods and held at or below 250, and it is at least `3 * FSR` (4 for FSR 1), the smallest the TIMING event accepts. FSR 7 with MA 30 plays 7/35.
- **Start.** `Network_Start_Timing` puts the choice over the game-speed table. A chosen MA replaces the tunnel's extra 20 frames.
- **Evaluation.** The master still evaluates every 256 frames. `Gamemd_Timing_Settings` uses the chosen FSR in place of the 10 or 5 and returns the chosen MA, so the TIMING event it sends repeats the choice. With only FSR chosen, MA still follows the round trip, aligned to the chosen FSR: through gamemd's formula from a launch file, and through the [measured look-ahead](#measured-look-ahead) from the built-in CnCNet lobby. A chosen option also applies to the never-measured round trip case (MA 250 under the chosen FSR).
- **Adaptive table.** It ignores the choice. The built-in CnCNet lobby writes `TimingPolicy=Gamemd` when the host chose anything.
- **Identity.** A chosen value is part of the session identity, so the out-of-sync report shows a mismatch. The identity of a match without a choice is unchanged.
- **Launch file defaults.** The CnCNet client forces `FrameSendRate=2` and `Protocol=0` in its game options and shows 7 and 2 as its host command defaults. A file with FSR 2 or 7, no `MaxAhead` and no `HostTiming=yes` is no choice, so stock-client games keep gamemd's timing. A host who wants exactly 2 or 7 through the stock client cannot get it; the built-in lobby writes `HostTiming=yes`.
- **LAN.** The host's choice travels in the `NET_GO` packet (`GlobalPacketType::ResponseTime.Options`, packed by `Pack_Host_Options`). A start with no choice is the short packet older builds send.

### Protocol 0

`Protocol=0` selects `COMM_PROTOCOL_SINGLE_NO_COMP`. The queue sends an uncompressed packet every frame, TIMING and NETWORK_REPORT events are refused, and the send period is 1. `Session::Reset_Network_Timing` sets MA to the chosen value, or to the game-speed start value, and the frame rate to the speed's rate. These stay for the match. Protocol 1 (single frame, compressed) is not offered.

## Measured look-ahead

A match from the built-in CnCNet lobby whose host chose a send period and no look-ahead (`/FrameSendRate` without `/MaxAhead`, under protocol 2) plays gamemd's timing with one change: the look-ahead follows the measured round trip instead of gamemd's formula. The lobby writes `MeasuredMaxAhead=yes` into every player's launch file. A typed `/MaxAhead` wins, and `/ProtocolVersion 0` turns it off. The stock CnCNet client never writes the key and LAN games never set it, so those games, and every match with no choice, keep the timing described above. The code is `NetTiming::Measured_Timing_Settings` and `NetTiming::Measured_Max_Ahead`.

### Rule

MA is the smallest whole number of send periods that holds the one-way trip, a 50 ms jitter margin (`MEASURED_JITTER_MARGIN`) and one send period. R is the worst smoothed round trip among seated players, in milliseconds, and F is the desired frame rate:

- Needed frames: ceil((R / 2 + 50) × F / 1000) + FSR.
- MA is that rounded up to whole send periods, at least the TIMING event's floor of 3 × FSR (4 at FSR 1), and at most 250 (`Host_Max_Ahead_For`).
- A rise applies in full at the next evaluation. gamemd's limit of one send period per evaluation does not apply.
- A fall must also hold for 1.25 × R, rounded up to a whole millisecond: MA falls to the value for 1.25 × R when that is below the current MA, and otherwise stays. The fall is staged as under [Applying a change](#applying-a-change).
- FSR stays the host's.
- The master evaluates at frames 64 and 128 as well as at every multiple of 256, so the start value is soon replaced. It skips an evaluation while a seated player has not reported a round trip, since a missing round trip would lower MA to the floor. A round trip that no connection has measured for 512 frames still gives MA 250.

At 60 fps:

| Round trip | FSR 1 | FSR 2 | FSR 3 | FSR 5 | gamemd's formula at FSR 3 |
| --- | --- | --- | --- | --- | --- |
| 40 ms | 6 (100 ms) | 8 (133 ms) | 9 (150 ms) | 15 (250 ms) | 12 |
| 76 ms | 7 (117 ms) | 8 (133 ms) | 9 (150 ms) | 15 (250 ms) | 15 |
| 100 ms | 7 (117 ms) | 8 (133 ms) | 9 (150 ms) | 15 (250 ms) | 15 |
| 150 ms | 9 (150 ms) | 10 (167 ms) | 12 (200 ms) | 15 (250 ms) | 18 |
| 220 ms | 11 (183 ms) | 12 (200 ms) | 15 (250 ms) | 15 (250 ms) | 21 |
| 300 ms | 13 (217 ms) | 14 (233 ms) | 15 (250 ms) | 20 (333 ms) | 24 |

An order waits MA frames plus up to FSR − 1 more for the next send frame. gamemd's column is the value after its rises; it climbs one send period per evaluation from the start value.

### What we reused

The cover is the adaptive table's: a rung must hold the one-way trip plus one send period, and never less than three send periods (`Select_Timing_Settings`). Both now compute the one-way trip with the same helper, and the fall's 1.25 × R is the adaptive table's drop headroom. The jitter margin is new. The early evaluation frames are the adaptive table's first two. We did not reuse the rung table itself, because a rung ties FSR to MA and the host has fixed FSR. Alignment, floor and ceiling come from `Host_Max_Ahead_For`, which a chosen look-ahead already uses.

### Margin

A machine may run until its frame reaches the oldest peer's last send frame plus MA (`Can_Advance`). That send frame is up to FSR − 1 frames old when it leaves the peer and a one-way trip old when it arrives. With both machines at one rate, MA must therefore exceed the one-way trip plus FSR − 1 frames, which the one send period covers. The 50 ms margin covers what the smoothed round trip does not show: single datagrams later than the average, and the wait loop, which checks again only after a tick and a map draw. In the real match the reported round trip moved between 75 and 99 ms, about 12 ms each way, and the relay runs below add up to 20 or 30 ms each way. A first version used a quarter of the round trip instead, which is 19 ms at 76 ms and shrinks on a short link although jitter does not; on the 100 ms relay link it chose 12 frames, and that run waited often. Rounding up to a whole send period adds up to FSR − 1 frames more. At FSR 3 and 60 fps, the floor of 9 frames (150 ms) holds every round trip up to 100 ms, which covers the real match.

We did not size the margin to cover a lost datagram. A loss waits for the retransmission, at least 100 ms (`MINIMUM_RTO`) after the send, and the measured runs below show such waits at gamemd's 15 frames too. Covering them would cost every order the same time.

### Start

gamemd's start is the game-speed table plus 20 frames through a tunnel: 40 + 20 = 60 frames at speed 0, one second. A chosen FSR 3 keeps 60, since it is a whole number of send periods. The first evaluation is at frame 256, and its TIMING event goes out on the next send frame and executes 60 frames later, so the real match below played its first 318 frames at 60.

With the measured look-ahead, the host's lobby sends its ping to the tunnel it took from the tunnel list. It sends twice that ping as the match's round-trip estimate (the `Ping` key of the start message), which takes each guest's leg to the tunnel to be as long as the host's. Every machine writes the estimate as `StartRoundTrip`, and the start settings are the rule above for that round trip at the game speed's frame rate. A lobby with no ping, because `Tunnel=` names the tunnel or the tunnel never answered, writes no estimate, and the match starts with gamemd's value. An estimate that is too low stalls the start until the evaluation at frame 64; one that is too high delays orders until then.

### Same values on every machine

Every machine writes `MeasuredMaxAhead` and `StartRoundTrip` from the host's room options and start message, so every machine starts with the same FSR and MA. Both keys are part of the session identity when `MeasuredMaxAhead` holds; the identity of every other match is unchanged. After the start, only the master evaluates and the TIMING event carries the result, as before.

### No choice

A host who chose nothing keeps the lobby's existing default, `TimingPolicy=Adaptive` (the `[CnCNet]` `TimingPolicy` setting in `RA2MD.INI`). The adaptive table already picks FSR and MA together from the measured round trip and starts on rung 2 (2/6, 100 ms at 60 fps), so we left the send period to it. With `TimingPolicy=Gamemd` in that setting, the match plays gamemd's timing exactly.

### Waits in the real match

These come from the host's log of a two-player match through the built-in lobby, at speed 0 with FSR 3 chosen (`DEBUG_19-11-2026_23-59-06.LOG`). The look-ahead was 60 frames to frame 318 and 15 frames (250 ms) after it, on a round trip of 75 to 99 ms.

- 71 of the 834 reports carried a longest wait of 50 ms or more, up to 366 ms. A report carries the larger of its own and the previous report's longest wait, so each wait shows in two reports: about 36 waits in 14 minutes. Player 0, the host, reported 52 of the 71.
- Waits are whole ticks of 1/60 second: 50 ms is 3 ticks and 133 ms is 8.
- Player 0 reported a wait of 16 or 33 ms in 306 of its 417 reports. It ran a little faster than player 1 and sat at the look-ahead's edge, waiting a tick or two at most send frames. A machine at that edge has no slack, so every late frame or late packet from the other machine becomes a wait of its full length, whatever the look-ahead.
- The waits of 50 ms or more are therefore most likely player 1's frames running long (a hitch on that machine) or a lost datagram waiting for its retransmission. A one-way trip of 40 ms cannot by itself account for a wait at a 250 ms look-ahead. The log has no packet timing, so it does not separate the two. The relay runs below with 20 ms of jitter and 1% loss show waits of 116 and 133 ms at gamemd's 15 frames.
- The wait loop draws the map on each pass (`Wait_For_Players`), so a wait ends only after a draw, which can lengthen it by one draw at a high resolution.

A lower look-ahead does not remove the slack that the leading machine never had, so these waits do not grow with it, except where jitter passes the margin.

## Launch file keys

| Key (`[Settings]`) | Effect |
| --- | --- |
| `GameSpeed` | Sets the frame-rate cap and the start look-ahead in the tables above. |
| `TimingPolicy` | `Adaptive` selects the [adaptive table](#adaptive-policy). Any other value, or no key, plays gamemd's timing. Each machine reads its own value, so the key is left out of the session identity. |
| `ReconnectTimeout` | Longest frame stall, in ticks, before the game gives up. Default 2400 (40 s). The value is clamped to 60 through 36000. |
| `ConnTimeout` | Longest wait for loading progress. Default 3600 (60 s). It does not apply to frame stalls. |
| `FrameSendRate`, `MaxAhead` | The host's send period (1 to 10) and look-ahead (4 to 250). See [Host choice](#host-choice). The CnCNet client's defaults (`FrameSendRate` 2 or 7 with no `MaxAhead`) count as no choice. |
| `HostTiming` | `yes` takes `FrameSendRate`, `MaxAhead` and `Protocol` literally, including the client's default values. The built-in lobby writes it. |
| `MeasuredMaxAhead` | `yes` plays the [measured look-ahead](#measured-look-ahead) under gamemd's timing. Read only without a chosen `MaxAhead` and under protocol 2. The built-in lobby writes it when the host chose a send period and no look-ahead. Part of the session identity when it holds. |
| `StartRoundTrip` | The lobby's round-trip estimate in milliseconds, 0 to 5000, for the start look-ahead. Read only with `MeasuredMaxAhead=yes`, and then part of the session identity. |
| `Protocol` | `0` plays protocol 0, only with `HostTiming=yes`. Any other value plays protocol 2. |
| `ParallelPaths` | `yes` searches unit paths in batches on several threads ([Parallel path searches](PERFORMANCE.md#parallel-path-searches)). Every player's file must hold the same value. `yes` is part of the session identity; a file without the key, or with `no`, keeps the identity it had. |
| `PreCalcMaxAhead` | Not read. |

## Comparison with gamemd

The reference is the decomp in `E:\Claude ra2 decomp\decomp`. We traced the protocol 2 path for the timing generator, the frame wait and the TIMING handler. The other call sites are noted where they matter.

| Area | gamemd | OpenYR, default | OpenYR, `TimingPolicy=Adaptive` |
| --- | --- | --- | --- |
| Start | Speed table, and +20 look-ahead when tunnelled (`OnGameStart`). | Same. | Rung 2 (2/6). |
| Evaluation | Every 256 frames, skipped while a process time is missing. | Same, and also skipped after 512 frames without a report. | Bootstrap at 64 and 128, then every 256. |
| Frame rate | Lower of speed and 1000 / worst process ms. | Same. | Same. |
| Send period | 10 at 31 fps and up, else 5 (online). | Same rule for online and LAN. | Rung table. |
| Look-ahead | One-way ticks times F / 120, plus the stage add-on, rise limited to one send period, rounded up, floor 3 send periods, ceiling 250. | Same. | Rung table from the round trip, with hysteresis and staged drops. |
| Stage | 1 by default. | 1. | Not used. |
| Fall of the look-ahead | Applied on execution. | Staged until the old horizon drains. | Staged. |
| TIMING event | F, MA (plus 10 for bit 0x1000), FSR. | Same format. The +10 is not read. | Same format. |
| Frame wait | 1000 / F whole ms, from frame start. | Same. | Solo deadline, exact average. |
| Online pacing (mode 4, MA ≥ 31) | +10 ms per threshold reached by the per-connection lag. | Same, with the peer frame lag. | None. |
| Tunnelled response term | Adds the largest per-connection value (default 20 ticks). | Not reproduced. | None. |

## Stall analysis

We read the two tester logs from `tester-inbox\files\mp-host-1014` and `mp-thunder-1014`. Both are from build `9737a743`, which predates the master fix. That build started at 30 fps and sent no timing event, so both copies ran with look-ahead 6 and send period 2 for the whole match. The order-delay lines show delays of 6 and 7 frames.

**What a stall line shows.** A `Network report` line carries the longest wait the copy spent in `Wait_For_Players` for another copy's frame, in the 128 frames since its last report. The host's report at frame 134 gives 783 ms, and thunder's gives 416 ms. A dialog that opens while the game waits counts toward that wait.

**The host's frame gaps are its own dialogs.** The host's gap of 637 ms between frames 118 and 119 contains a load of `gameopt.rml`, which the log reports as "opened over 3090 passes in 1109 ms". Its gap of 875 ms between frames 164 and 165 ends as `display.rml` opens. Thunder kept its frames at about 30 ms during those gaps, so it waited for the host's frame data. The `Demand loading` lines in both logs do not fall inside the stall windows.

**Two of thunder's gaps have no local cause.** Thunder's gaps of 754 ms (frame 124 to 125) and 936 ms (frame 170 to 171) have no dialog or load line in its log. The host ran about 190 ms per frame over frames 123 to 125, so both copies were waiting on each other's send-period packets within a 200 ms horizon. The logs do not record packets, retransmissions or their timing, so we cannot say why the packets were late.

**Would the change remove them?** Only partly, and we have not measured it on a real link. The new policy at speed 0 with an 83 ms round trip starts at 60 frames (1 s, through a tunnel) and drops to 30 frames (500 ms at 60 fps) at frame 256. The old look-ahead was 6 frames (200 ms at 30 fps). A copy's pause shorter than its look-ahead no longer stalls the other copy. A longer pause stalls the other copy only for the excess. On that estimate the host's 637 ms and 875 ms pauses would stall thunder for about 140 ms and 375 ms, where the old look-ahead would have given about 440 ms and 675 ms. These figures come from the horizon arithmetic. The loopback runs below contain no pause, so they do not test this case. With ±30 ms of jitter, the longest report wait fell from 50 ms to 0 ms, but the jitter is not the real link's, and one run each does not establish it.

## Measurements

### Measured look-ahead runs

These runs used `scripts/run_netsync.ps1 -Tunnel -RelayBuffer 2048 -GameSpeed 0 -FrameSendRate 3 -Seconds 120` with the XMP29U2 map and the `xmp29u2` order scripts. "Before" is the build without the measured look-ahead; "after" adds `-MeasuredMaxAhead -StartRoundTripMs 80`, which writes the keys the built-in lobby writes. Frames is the frames side A played in the 120 s. A wait is a report's longest wait; one wait shows in two reports. Each row is one run, and other games ran on the computer during some of them.

| Relay hop | Run | Look-ahead | Order delay (frames) | Frames | Longest wait | Reports with 50 ms or more | Mean report wait |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 40 ms | Before | 60 to frame 318, then 15 | 15 to 17, 60 at the start | 7307 | 16 ms | 0 of 116 | 0.3 ms |
| 40 ms | After | 9 throughout | 9 to 11 | 7402 | 0 ms | 0 of 118 | 0.0 ms |
| 40 ± 20 ms, 1% loss | Before | 60 to frame 318, then 15 | 15 to 17, 60 at the start | 7109 | 133 ms | 2 of 114 | 2.6 ms |
| 40 ± 20 ms, 1% loss | Before, again | 60 to frame 318, then 15 | 15 to 17, 60 at the start | 7089 | 116 ms | 12 of 114 | 10.2 ms |
| 40 ± 20 ms, 1% loss | After | 9, then 12 from frame 777 | 9 to 14 | 7095 | 133 ms | 4 of 114 | 6.5 ms |
| 40 ± 20 ms, 1% loss | After, again | 9, then 12 from frame 75 | 9 to 14 | 7393 | 216 ms | 8 of 118 | 11.1 ms |
| 100 ± 30 ms, 2% loss | Before | 60 to frame 318, then 21 | 21 to 23, 60 at the start | 7372 | 200 ms | 6 of 118 | 8.0 ms |
| 100 ± 30 ms, 2% loss | After | 9, then 15 from frame 75 | 9 to 17 | 7230 | 566 ms | 16 of 116 | 35.9 ms |

Every run printed `IN SYNC`, and no order arrived too late. On the real match's link the order delay falls from 15 frames (250 ms) to 9 (150 ms) with no added waits. With 20 ms of jitter and 1% loss the waits of the two look-aheads overlap from run to run, and the loss waits reach 100 to 200 ms at either. On the 200 ms link with 30 ms of jitter and 2% loss the lower look-ahead waited about four times as long on average, for orders 6 frames sooner. Two more runs of the 40 ± 20 ms link with the measured look-ahead ran at 26 to 34 fps for their first 2000 frames on both copies, with the round trip reading 150 to 160 ms instead of about 110; other games were starting on the computer then, and a repeat ran at 62.5 fps throughout. We count those two as the computer's load, not the policy.

A run with `-MeasuredMaxAhead` and no `-StartRoundTripMs` started at 60, as gamemd does. The reports sent at frame 32 arrived after frame 64, so the evaluation at frame 64 waited, and the one at frame 128 sent 3/9, which applied at frame 189. The `lookahead` order scripts (`tests/netsync/lookahead-side-a.txt` and `-b.txt`) with the estimate stayed at 9 frames, with delays of 9 to 11 frames and no wait.

### Latency runs

These runs used `scripts/run_netsync.ps1 -Tunnel -LatencyMs 50 -RelayBuffer 2048 -Seconds 150` with the XMP29U2 map and the autotest order scripts. The relay adds 50 ms in each direction, so each round trip was about 122 to 130 ms, with no jitter. FPS is the frame count over the logged frame lines. The steady gap is the longest gap between consecutive frames after frame 1, so the start is left out. Each row is one run.

| Run | Game speed | Host side | Common frames | FPS (A / B) | Steady gap (A / B) | Longest report wait | Look-ahead |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Before | 1 | 0 | 6579 | 44.8 / 44.7 | 35 ms / not kept | 33 ms | 6 (rung 2) |
| After | 1 | 0 | 6655 | 45.4 / 45.4 | 34 ms / 33 ms | 16 ms | 60 to frame 255, then 30 |
| Before | 0 | 8 | 8725 | 59.5 / 59.5 | 36 ms / 36 ms | 16 ms | 9 (rung 3) |
| After | 0 | 8 | 9216 | 62.4 / 62.4 | 26 ms / 26 ms | 0 ms | 60 to frame 255, then 30 |

"Before" is the build at the base commit, `fc9c7cec`, which has the master fix and the adaptive table. Every run printed `IN SYNC`. An earlier run of the changed build at speed 0 with the same settings reported a 16 ms longest wait. That build did not yet compute the peer lag that the pacing reads.

The rates sit a little above the speed's target. gamemd's whole-millisecond wait allows 62.5 fps at 60 and 45.5 fps at 45, and the runs follow that wait. The start gap from frame 0 to 1 was 35 to 124 ms before, and 52 to 169 ms after.

With a fixed delay and no pauses, neither policy stalls. The longest report wait was 0 to 16 ms in every run after the change. The new look-ahead of 30 frames is 500 ms at 60 fps, against 150 ms before at speed 0. Jitter and a local pause are the cases where the difference would show, and the jittered runs below test that.

### Jittered runs

These runs add `-JitterMs 30` to the same relay, so each datagram is delayed by 50 ms plus or minus 30 ms in each direction. Both ran at game speed 0 with the host in seat 8, which is the owner's case. Both copies run the pacing in the changed build, but the logs do not record when the pacing waits fired.

| Run | Common frames | FPS (A / B) | Steady gap (A / B) | Longest report wait | Look-ahead | Datagrams the relay forwarded |
| --- | --- | --- | --- | --- | --- | --- |
| Before | 8719 | 59.5 / 59.4 | 35 ms / 47 ms | 50 ms | 9 (rung 3) | 6113 |
| After | 9219 | 62.4 / 62.4 | 28 ms / 38 ms | 0 ms | 60 to frame 255, then 30 | 2123 |

Both runs printed `IN SYNC`. The start gap from frame 0 to 1 was 142 and 177 ms before, and 134 and 169 ms after. The after run sends fewer datagrams, because at speed 0 it sends every 10 frames where the rung-3 policy sent every 3. Both runs use relay seed 1, but they send different numbers of datagrams, so they draw different delays. Each is a single run. An earlier run of the changed build with the same settings had a 16 ms longest report wait.

### Loopback runs, adaptive policy

#### Setup

`scripts/run_netsync.ps1 -Build agent18`, map XMP29U2, seed 1, two human seats that send no input,
two computer players, loopback (`127.0.0.1`), `GameSpeed=1` (45 fps cap).

Orders come from autotest `order` steps, which queue networked mission events. The `move` step
assigns the mission on the local objects only. It is not networked, so it cannot drive a two-copy
test.

#### Results

| Run | Length | Orders | Order delay (frames) | Wall-clock issue to execution | Result |
| --- | --- | --- | --- | --- | --- |
| 1 | 120 s, 5016 common frames | 40 per side, every 120 frames from frame 120 | 4 for all 80 | mean 97 ms, range 88 to 268 ms (both sides) | In sync |
| 2 | 20 s, 748 common frames | 50 per side, consecutive frames 12 to 40 and 60 to 80 | 6, 7, 5, 4 (see below) | not measured | In sync |

Run 1 details:

- The policy started on rung 2 (2/6). The census at frame 64 had a 1 ms round trip, so it chose rung
  1. The drop was sent on frame 70 and took effect on frame 76 (run 2 shows the frame-76 change).
- The policy stayed on rung 1 for the rest of the run. It logged 20 evaluations.
- Every executed order ran on its scheduled frame. Both copies logged 80 executions.
- The frame rate averaged 42.9 fps against the 45 fps target (23.3 ms per frame).
- The largest delays, 223 ms and 268 ms, were issued on frames 3840 and 3960. The frame-4096 report
  records a 183 ms wait in that stretch.

Run 2 shows the send-frame wait at FSR 2 and the drop to FSR 1:

| Issue frame | Delay (frames) | Reason |
| --- | --- | --- |
| Even frames from 12 to 74 (none from 41 to 59) | 6 | Issued on a send frame. FSR 2, MA 6. |
| Odd frames from 13 to 73 | 7 | Waits one frame for the next send frame. |
| 75 | 5 | Sent on frame 76, where MA had dropped to 4. |
| 76 to 80 | 4 | FSR 1, MA 4. |

Every execution in run 2 ran on its scheduled frame.

#### Policy output from the formulas

These values come from calling `Select_Timing_Settings` in `code/nettiming.cpp` with the same
inputs. Each cell is the rung chosen when the round trip rises, as FSR/MA, then the minimum delay
in milliseconds. The delay can be up to FSR − 1 frames longer.

| Round trip (ms) | 30 fps | 45 fps | 60 fps |
| --- | --- | --- | --- |
| 0 | 1/4, 133 ms | 1/4, 89 ms | 1/4, 67 ms |
| 50 | 1/4, 133 ms | 1/4, 89 ms | 1/4, 67 ms |
| 100 | 1/4, 133 ms | 1/4, 89 ms | 1/4, 67 ms |
| 150 | 1/4, 133 ms | 2/6, 133 ms | 3/9, 150 ms |
| 200 | 1/4, 133 ms | 3/9, 200 ms | 3/9, 150 ms |
| 300 | 3/9, 300 ms | 4/12, 267 ms | 5/15, 250 ms |
| 400 | 3/9, 300 ms | 5/15, 333 ms | 6/18, 300 ms |

At 50, 100 and 200 ms with 45 fps the choice is rung 1, rung 1 and rung 3. Rung 1 keeps a 4-frame
delay until the round trip passes 133 ms.

## Log lines

Under `-SYNCLOG`, the copies write three kinds of line to their debug logs:

- `SYNC timing frame F rtt_ms R rtt_complete C stall_ms S process_ms P fps G policy FSR/MA changed X`
  is written by the master at each timing evaluation.
- `SYNC order issued I sent S executes E delay D type T house H` is written by the sending copy
  for each order.
- `SYNC order executed frame X scheduled E type T house H` is written by every copy when an order
  runs.

All three need `-SYNCLOG`. The order lines skip frame-sync, frame-info, response-time, process-time,
timing and network-report events.

## Proposal: adaptive policy, order delay that follows measured ping

This is a plan. Nothing here is implemented.

The aim is an order delay close to one-way time plus one send period plus a margin for jitter,
with the same frame model and values that every machine agrees on.

### What to change

1. **Lower the floor for links with small jitter.** The 3-send-period floor adds two periods over the
   minimum. At 45 fps, every round trip from 0 to 133 ms gets 4 frames. Test a floor of two send
   periods plus a jitter margin. Keep the current floor as the default until tests show the stall
   cost.
2. **Account for jitter.** `RttEstimator` keeps `RttVariation`, but the census carries only the
   smoothed round trip. Report the smoothed value plus k × variation, with k chosen by tests. The
   retransmit timeout uses k = 4, which lets a 30 ms jitter add 120 ms to the value, so test smaller
   values first. A new meaning for the reported value needs a version gate, because older builds
   read the same field.
3. **Raise faster.** Evaluate rises on each report, every 128 frames, and keep the drop cadence. Check
   for flapping, because hysteresis now covers drops only.
4. **Consider gamemd's growth limit.** MA could rise at most one FSR per evaluation. It would slow the
   response to a large spike. Test whether the current immediate rise is safe before adopting it.
5. **Keep the schedule out of local decisions.** A machine must not change its own FSR or MA from its
   own measurements. Local measurements feed only the census.
6. **Local pacing is optional.** Adding gamemd's 10 ms waits changes only wall-clock time. The
   schedule stays the same, so the waits cannot desync by themselves.

### Risks

- **Stalls.** A horizon shorter than one-way time plus a late packet's wait makes the game wait,
  even when nothing is lost. The 120-second run recorded a 183 ms wait on loopback, where no network
  delay exists. The floor is the protection. Each candidate floor needs a measured stall count.
- **Late orders.** A packet that arrives after its execution frame is reported as too late
  (`Execute_DoList`). A shorter horizon raises that risk. The rise window covers only the changes
  made through the `TIMING` event. It does not cover packet loss.
- **Desync.** Every machine must hold the same FSR, MA and activation frame. Only the master
  computes the schedule, and the `TIMING` event carries the result. A change that lets one machine
  set its own MA would let machines execute the same order on different frames.
- **Master handoff during a drop.** `Prepare_Network_Timing_Master` restarts the policy from the
  current target. A drop that is staged when the master leaves must complete the same way on every
  machine.
- **Mixed builds.** A policy-only change keeps the packet format, so it needs no version gate by
  itself. Test a mixed pair before shipping. A new field in `TIMING`, or a new meaning for a reported
  value, needs a version gate. Bump `REVISION` only if serialized data changes.
- **Frame rate.** The desired rate follows the slowest machine's process time. A slow machine raises
  the millisecond delay for everyone. This does not desync.

### Tests

- **Unit tests.** Extend `tests/nettiming` with the rung limits (4000 × r / F and 6000 / F) and the
  1.25 × R drop threshold. Add the staged-drop activation and the master handoff.
- **Latency switch, test builds only.** Add a command-line option that holds each outgoing datagram
  in `code/netsocket_win32.cpp` and `code/netsocket_posix.cpp`, with a fixed delay and seeded
  jitter. Keep order. The switch is off by default. The runner passes it to each copy on the command
  line.
- **Matrix.** Run symmetric links at 25 and 50 ms per side (round trips of 50 and 100 ms), an
  asymmetric pair, and 0 and 10 ms of jitter, each for at least 400 seconds, at 45 fps. Script orders
  on consecutive frames, as in run 2, to measure the send-frame wait at each FSR.
- **Acceptance.** Every order runs on its scheduled frame. Each delay lies between MA and MA + FSR − 1
  for the rung logged at that frame. The rung matches the table within the evaluation cadence. The
  run stays in sync. Stall counts and durations are no worse than the baseline. No order arrives too
  late.
- **Negative control.** `-ForceDesyncB` must still be detected.

## Open questions

- **LAN send period.** gamemd's LAN games keep the send period they started with, which is 10 at speeds 0 and 1 and 5 otherwise. We apply the online rule to LAN games too, so the two differ only when the process time pulls F below 31 at speed 0 or 1. We have not traced the LAN start path (`Net2Remote_Connect` may set 5), so we do not know which start value a LAN game gets.
- **Tunnelled response term.** The per-connection values that gamemd adds in a tunnelled game (`0x45`) default to 20 ticks, and their writer is not identified. The table holds them at `DAT_00afa368` with stride 24 bytes, and the lobby's `unknown_8AA[0x41]` gives the packet-router flag (`0x20f`). We take the spawner's `[Tunnel]` section as that flag. Until the writer is found, tunnelled games keep a shorter look-ahead than gamemd's.
- **Per-connection pacing values.** The 10 ms waits read `unknown_2854[0x3c + i]`, and nothing in the decompile that we read writes that array. We use the frames the most-behind connected peer trails this machine instead.
- **Start look-ahead on the precalculated path.** gamemd sends a precalculated timing event at frame 0. `PrecalcMaxAhead` is set on the ping-unknown path, and inside a latency branch of the chat sink. We did not find the assignment for a normal start, so we use the speed table.
- **Latency stage.** gamemd's stage 2 applies when the start ping is above 1000 ms, and the lobby's latency event can change it. Neither is ported. The stage is 1.
- **Process-time guard.** gamemd computes the frame rate's process term only when `DAT_00afa400 & 0xffff` is above 4. Otherwise the decompile leaves the frame rate unset. We did not resolve what gamemd uses in that case.
- **Packet timing in the tester logs.** The 754 ms and 936 ms gaps on thunder, and the host's ~190 ms frames around frame 124, have no recorded cause. A log of packet arrival times would show whether they are losses, retransmissions or a late send.
- **Fall of the look-ahead.** gamemd applies a fall on execution. We stage it, and the staged horizon is shorter than gamemd's during the drain. Whether an immediate fall is safe on a real link is untested.
- **The lobby's policy on each machine.** The built-in lobby writes `TimingPolicy` from each machine's own `[CnCNet]` setting when the host chose nothing. Two machines with different settings start with different look-aheads (2/6 against 10/60), so a guest could pass the host's orders before they arrive. The default setting is the same on every machine, so this needs an edited `RA2MD.INI`. Sending the host's policy in the start message would close it.
- **Start estimate.** `StartRoundTrip` takes each guest's leg to the tunnel to equal the host's. A guest's own tunnel ping would need a new room message.
- **Real links.** Every run here used loopback with a fixed 50 ms delay each way. We have not run jitter, loss or a real internet route under the new policy.
