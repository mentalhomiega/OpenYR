# Rollback netcode

Rollback lets each machine run its own orders at once and predict the other
player's. When a remote order arrives for a frame already played, the machine
restores the game state from before that frame and re-runs up to the present
with the order in place. This hides the order delay that the lockstep schedule
in [Network timing](NETCODE.md) needs.

This page is a plan. Only the input link, the game-state snapshot and the
`roundtrip` and `snaptrip` autotest steps exist. No game code uses the link or
the snapshot, and the steps run only from an autotest script, so no other game
played on this branch behaves differently from `modern`.

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
   and load it back on a rollback. The full save round-trips the game state,
   but its load takes about 1.6 s. The game-state snapshot below keeps the
   theater, art, rules and rule types in place and restores in about 46 ms
   (see [Save and load measurements](#save-and-load-measurements)). That is
   still several frames' time, so this way needs a faster restore before it can
   run on every rollback.
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

### Save and load measurements

The autotest step `roundtrip` saves the game to memory, loads it back in place
of the running game and saves it again ([AUTOTEST.md](AUTOTEST.md)). These
figures come from one Win32 Release build on one PC: a skirmish on
`XMP29U2.MAP`, seed 1, game speed 0, the player and three computer players,
with 105 to 150 buildings, vehicles and soldiers. The player only deploys its MCV, and the game ends on
frame 7478 when the player loses.

- **The game plays on the same.** A run with a round trip every 300 frames
  (24 in all) logged the same `hash` every 150 frames as a run without them,
  through frame 7478, and lost on the same frame. Two runs without round trips
  matched each other. `hash` covers each object's position, facing, strength,
  mission, limbo state and owner, each house's credits and the random-number
  state.
- **The saved state comes back nearly the same.** On frame 600, a state dump
  before the round trip and one after it first differ in the scenario's
  elapsed play time, a wall-clock count that runs on while the load takes
  place. Four civilian building types (CASYDN01, CATIME, CALA02, CALOND02)
  also have a longer record after the load: their `UINameLabel`, empty before,
  holds the type's ID. That is interface text. Records of equal length were not
  compared byte by byte for this full round trip.
- **Saving costs 20 to 50 ms**, 24 to 26 ms on average, for 5.2 to 5.3 MB of
  state.
- **Loading costs 1.3 to 1.9 s**, 1.6 s on average. Most of it rebuilds what a
  match never changes. The theater takes about 0.34 s before its tile sets are
  read, and the map's load reloads 146 isometric tiles; reading the map takes
  0.6 to 0.9 s in all.

#### Game-state snapshot

The autotest step `snaptrip` does the same with the rollback snapshot
(`Round_Trip_Game_State` with `snapshot`). The snapshot leaves out the rules,
the sides, and the types rules and art define: animation, house, vehicle,
infantry, building, aircraft, voxel animation, warhead, weapon, particle,
particle system, projectile, smudge, overlay, super weapon and terrain types.
They stay in place across a restore, and the snapshot numbers them first, so
the objects' pointers to them resolve without their records. The theater, art,
rules and isometric tiles are not reloaded either. The snapshot still holds
the types a map defines (task forces, team, script, tag, trigger and AI trigger
types) and the tiberiums, which hold growth and spread state.

Same game and build as above, with a snapshot round trip every 300 frames:

- **The game plays on the same.** All 50 hashes match the run without round
  trips, through the loss on frame 7478.
- **The state comes back the same except for bookkeeping.** On frame 600, full
  state dumps before and after a snapshot round trip have the same size and the
  same 10,320 records. 44 records differ:
  - the scenario's elapsed play time, as above;
  - the map's `Redraws` count, which the restore raises to redraw everything;
  - `Dirty` in 40 drive locomotors, which writing a record clears, so the first
    dump itself changed it;
  - `IsRadarTracked` in 2 buildings, which `Post_Load_Radar_Fixup` clears on
    every object so that the radar registers each one again.
- **The snapshot is 2.7 to 2.8 MB.**
- **Saving costs 13 to 22 ms**, 16 ms on average. Writing the map's cells is
  12 ms of that on average.
- **Restoring costs 31 to 145 ms**, 46 ms on average: 18 ms deleting the running
  game's objects, 14 ms reading the map's cells, 7 ms resolving pointers and
  the fixups after, and the rest reading the other objects.

A frame at 60 fps lasts 16.7 ms. Two costs dominate, and both come from the
restore recreating every object:

- The map's 7,140 cells are about 1.7 MB of the snapshot and three quarters of
  the save. Only a few change each frame, but a cell that holds an object holds
  its address, and every object gets a new one in a restore. So writing only the
  changed cells needs objects restored at their old addresses, or every cell's
  object pointers rewritten.
- Deleting the running game's objects, 18 ms on average, runs every object's
  destructor before the snapshot's copies are built.

Restoring objects in place, into the objects that already exist, would avoid
both the deletion and the new addresses. That is close to option 2 below.

## Open questions

- Whether any rule type changes during a match on other maps or in campaign
  missions. The snapshot assumes none does.
- Whether other maps, campaign missions and games with player orders round-trip
  the same way. Only one skirmish has been measured.
- Which globals hold game state, and which threads touch it.
- How chat, pause and exit, which ride vanilla packets, travel in a rollback
  match.
- What happens to a player who leaves: the prototype ignores their house.
