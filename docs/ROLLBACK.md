# Rollback netcode

Rollback lets each machine run its own orders at once and predict the other
player's. When a remote order arrives for a frame already played, the machine
restores the game state from before that frame and re-runs up to the present
with the order in place. This hides the order delay that the lockstep schedule
in [Network timing](NETCODE.md) needs.

This page is a plan. Only the input link exists, and no game code uses it, so
no game played on this branch behaves differently from `modern`.

## What exists

`code/rollbacklink.h` and `code/rollbacklink.cpp` hold `RollbackLinkClass`,
the datagram link between the two machines of a rollback match. The `rollbacklink`
test covers it without game assets.

- **Inputs.** Each order is an opaque byte string of up to 512 bytes, with the
  frame it runs on and a sequence number. The receiver takes inputs in sequence
  order only; one that arrives after a gap waits for the resend.
- **Sending.** An input goes in two datagrams in a row, then again only when no
  acknowledgement has come within the smoothed round trip plus four deviations
  plus 30 ms (250 ms before the first measurement).
- **Through frame.** Each datagram says the frame through which the sender has
  queued every input. The receiver accepts that frame only once it holds every
  input the claim rests on, so a lost datagram cannot complete a frame early.
- **Frame hashes.** The newest 32 hashes (8 through a tunnel) ride on every
  datagram for the sync check.
- **Timing figures.** Each datagram carries the sender's frame advantage and a
  late report for time sync and input delay, plus send and echo times for the
  round trip. Lost datagrams are counted from the gaps in their numbers.
- **Farewell.** A side that exits or stops waiting sends its reason three
  times.
- **Tunnels.** A tunnelled datagram starts with the sender's and receiver's
  ids, either as a CnCNet v3 tunnel's 32-bit little-endian ids or as the
  game's 16-bit network-order ids. A tunnelled datagram stays under 1000 bytes.
- **Test lag.** `Set_Simulation` holds arriving datagrams for a fixed lag plus
  seeded jitter, without reordering them, and drops a set percentage.

The wire format is the link's own (magic `RBO1`, all fields little-endian). It
does not interoperate with the yrpp-spawner prototype, whose inputs are raw
gamemd `EventClass` bytes.

## Where the design comes from

The design was proven in a prototype built into the CnCNet spawner for
gamemd 1.001 with Mental Omega 3.3.6. These results are from that prototype,
not from OpenYR:

- Restoring a snapshot reproduced the game exactly: 0 hash mismatches over
  33,000 replay frames with 1,090 rollbacks of 6 frames, and in stress runs of
  30-frame rollbacks.
- Two linked copies played 6,000-frame live matches at 0 and 50 ms of added
  lag with 0 hash mismatches, on one PC and between two PCs.
- Real matches through CnCNet tunnels ran over 20,000 frames in sync.
- On emulated links matching real opponents' (50 ms with 30 ms jitter, and
  55 ms with 35 ms jitter turning congested), rollback held 57-61 fps with
  100-130 ms of order delay. Lockstep needed 180-260 ms to come close.

## Port plan

Each step keeps rollback off unless a launch file asks for it, so the default
game, its packets, saves and replays stay as they are.

| Prototype part | What it did in gamemd | OpenYR plan |
| --- | --- | --- |
| `Rollback/Net` | UDP link | Done: `RollbackLinkClass` on `SocketClass`. |
| `Rollback/Arena`, `Snapshots` | Moved gamemd's and Ares' heap into a write-watched region, then copied dirty pages and the writable image sections | Open question, below. |
| `Rollback/Locks`, `Files` | Repaired critical sections and file handles that a whole-image restore had rolled back | Not needed if the snapshot covers game state only. |
| `Rollback/Harness` | Frame loop: snapshot at frame start, inject both sides' inputs, restore and re-run, skip drawing, sound and input while re-running | Port into `queue.cpp` and the main loop, behind the launch key. |
| `Rollback/SpawnSwitch` | Turned rollback on from `[Settings] Rollback=True` for exactly two humans and no observers | Read in `spawnerconfig.cpp`. |
| Time sync, adaptive input delay | Paced the side that ran ahead; set input delay from the measured link | Port with the harness; keep each tuning value as a launch key. |
| `Rollback/TestPlayer` | Scripted player for long tests | Optional; the `tests/netsync` scripts may cover it. |

### Snapshots

The prototype could not see gamemd's state, so it snapshotted whole memory
regions. OpenYR's state lives in its own pools and globals, which allows a
narrower snapshot. Three ways to try, in order:

1. **Save into memory.** Write the game to a memory stream with the save code
   and load it back on a rollback. This reuses tested code, but the save must
   round-trip exactly, and a save every frame must cost a few milliseconds at
   most. Measure both first.
2. **An arena for game-state allocations.** Give the object pools and other
   game-state allocations their own region and copy its dirty pages, as the
   prototype did. Globals that hold game state need their own list; renderer,
   audio, UI and socket state must stay out.
3. **Per-class copies.** Copy each heap and global explicitly. This is the most
   work and the easiest to get wrong as classes change.

Whichever is chosen, the check is the prototype's: restore during replay
playback and compare the frame hash after every re-run.

### Checks for each step

- The campaign autotests keep their frames: `tests/campaign/win-sov05.txt`
  wins at frame 1275 and `win-all01.txt` at frame 3042.
- Two copies under the netsync runner stay in sync with rollback off, and with
  it on once the harness exists.
- Every test copy has `ReportUpload=never` under `[Options]` in `RA2MD.INI`.

## Open questions

- Whether the save code round-trips a game exactly, and how long it takes per
  frame on a large map.
- Which globals hold game state, and which threads touch it.
- How chat, pause and exit, which ride vanilla packets, travel in a rollback
  match.
- What happens to a player who leaves: the prototype ignores their house.
