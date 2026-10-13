# Network timing and order delay

An order in a network game runs `Session.MaxAhead` (MA) frames after the next send frame. A send
frame is a frame that is a multiple of `Session.FrameSendRate` (FSR). FSR and MA come from a timing
policy that runs on one player's machine. Every machine applies the policy's result on the same
frame, so all machines execute each order on the same frame.

On a two-copy loopback match the policy settles on FSR 1 and MA 4 after frame 76, which gives a
4-frame order delay (89 ms at 45 fps). Loopback has no network delay, so these numbers show the
policy's floor only. The table under [Policy output from the formulas](#policy-output-from-the-formulas)
gives the choice at other round trips.

This page covers protocol 2 (`COMM_PROTOCOL_MULTI_E_COMP`). Every assignment to
`Session.CommProtocol` in this build uses it, so the protocol 0 and 1 branches do not run in a
network game.

## Order delay

An order issued on frame F executes on frame E = B + MA, where B is the first send frame at or after
F. The delay E − F is MA plus between 0 and FSR − 1 frames. MA is a multiple of FSR in every valid
setting, so E is a send frame too.

- The order enters `OutList` with the frame it was created in. `EventClass` constructors set
  `Frame` to `::Frame`.
- Every frame runs the network step. It returns early on frames that are not send frames
  (`Process_Send_Period`). Game logic runs on every frame.
- On a send frame, `Add_Compressed_Events` sets each queued order's frame to E, moves it to
  `DoList`, and sends it.
- Each machine runs `Execute_DoList` for E.

An order created before the network step of frame F can go out in frame F. The 120-second run
below shows this: an order created on frame 120 went out on frame 120.

### Frame advance

A machine moves past a send frame when both of these hold (`Can_Advance`):

- Its frame is below the oldest peer frame plus MA. A peer's frame is its `FRAMEINFO` frame minus
  its `Delay` field, which is the peer's frame at its last send.
- It has received every order that each peer reports as sent.

When either check fails, `Wait_For_Players` services the connections and resends the frame-sync
packet about once a second (`TIMER_SECOND`, 60 ticks). It also draws the stall marker. If the wait
passes `ReconnectTimeout`, `Handle_Timeout` runs. If that does not clear the stall, the game reports
the player as not responding.

## Timing values

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

### How a change takes effect

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

In a network game, the desired frame rate F is the smaller of the game-speed rate and 1000 divided by
the longest average process time among seated players. Process time runs from the start of
`Main_Loop` to the end of AI logic. It includes the map render and excludes the network wait. An
average of 0 ms gives 60 fps. While a seated player's process report is missing or stale, F keeps its
synchronised value.

| `GameSpeed` | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Frame rate | 60 | 45 | 30 | 20 | 15 | 12 | 10 |

Other values give 60 fps. Each frame waits 1000 / F ms. `FramePacer` spreads the remainder, so the
average rate is exact. The game starts at 30 fps until the first `TIMING` event.

MA counts frames, so its time depends on F. The rung-1 delay of 4 frames is 133 ms at 30 fps, 89 ms
at 45 fps and 67 ms at 60 fps. A slow machine lowers F for every player, so the same MA takes longer
in milliseconds.

## Launch file keys

| Key (`[Settings]`) | Effect |
| --- | --- |
| `GameSpeed` | Sets the frame-rate cap in the table above. |
| `ReconnectTimeout` | Longest frame stall, in ticks, before the game gives up. Default 2400 (40 s). The value is clamped to 60 through 36000. |
| `ConnTimeout` | Longest wait for loading progress. Default 3600 (60 s). It does not apply to frame stalls. |
| `Protocol`, `FrameSendRate`, `MaxAhead`, `PreCalcMaxAhead` | Not read. `tests/spawner/spawncontract.cpp` checks that they do not change the match identity. |

## Comparison with gamemd

The reference is the decomp in `E:\Claude ra2 decomp\decomp`. We traced the protocol 2 path, not
every call site.

| Area | gamemd | OpenYR |
| --- | --- | --- |
| Start | FSR 5. MA is the response time divided by 8, rounded up to a multiple of 5, or 10 when that value is below 11 (`Net2Remote_Connect`). | FSR 2, MA 6 until the first evaluation (rung 2). |
| Order frame | Ceiling of (frame + MA) to a multiple of FSR. | Same. |
| Frame advance | MA is compared with peer frames in `Wait_For_Players`. Not traced line by line. | `Can_Advance` checks the same two conditions. |
| Evaluation | Every 256 frames (frame number low byte 0), and when a precalculated value is pending. | Every 256 frames after bootstrap. |
| Target | One-way frames = response time × F / 120. An escalation stage of 0 to 3 scales the response time by 1, 1.5, 2 or 3 and adds 0, 10, 20 or 30 frames. MA rises at most one FSR per evaluation. Minimum 3 FSR. | Rung table from R in ms, with hysteresis and staged drops. MA can jump several rungs at once. |
| Timing event | Sets F, MA (minus 10 when special-flags bit 0x1000 is set) and FSR. | Sets F, MA and FSR. No −10 adjustment. |
| Frame rate | min(1000 / largest process ms, game-speed rate). | Same. |
| Frame timer | 1000 / F ms, truncated to whole milliseconds. | `FramePacer`, exact average. |
| Local pacing | In game mode 4 with MA of at least 31, adds 10 ms to a frame's wait for each of MA/4, MA/2 and 3MA/4 that a per-player value reaches. | None. |
| Fixed table | `CChatEventSink::OnGameStart` sets MA by game speed (40, 40, 30, 20, 20, 20, 10) and F by speed (60, 45, 30, 20, 15, 12, 10). FSR is 10 for speeds 0 and 1, else 5. | No fixed table. |

The order stamp and the execution frame match gamemd's protocol 2 path. Our frame-advance check uses
the two conditions listed above. We did not trace gamemd's check line by line. The timing differs:
gamemd derives MA from the response time with an escalation schedule and a per-evaluation growth
limit. We pick from a rung table and add hysteresis and staged drops.

Protocol 0 is not reached in this build, so its timing code has no effect on network games. Gamemd's
protocol 0 branch uses FSR 5. Our `netdlg2.cpp` branch for the same case uses FSR 3.

## Measurements

### Setup

`scripts/run_netsync.ps1 -Build agent18`, map XMP29U2, seed 1, two human seats that send no input,
two computer players, loopback (`127.0.0.1`), `GameSpeed=1` (45 fps cap).

Orders come from autotest `order` steps, which queue networked mission events. The `move` step
assigns the mission on the local objects only. It is not networked, so it cannot drive a two-copy
test.

### Results

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

### Policy output from the formulas

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

## Proposal: order delay that follows measured ping

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

- Which gamemd path runs for a LAN game and for an Internet game: `Net2Remote_Connect` (FSR 5 and an
  RTT-based MA) or `CChatEventSink::OnGameStart` (the fixed table). Neither call site is traced yet.
- What raises gamemd's stage counter (0 to 3), which adds 0, 10, 20 or 30 to the target.
- What writes the per-player values at `unknown_2854[0x3c]` that gamemd's local 10 ms waits read.
- What special-flags bit 0x1000 does to the timing event (MA minus 10 in gamemd).
- Whether the delay follows the table above at FSR above 2 on a real link. Loopback reached FSR 2
  only in the first 76 frames.
- Whether the 183 ms wait on loopback is host scheduling alone. The run did not record CPU load.
- The jitter that real links show. The testers' `-SYNCLOG` files can provide it.
