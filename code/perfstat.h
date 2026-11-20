/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Timing and counts for the game frame, the draw and the present. Recording is off
// unless a user turns it on: the Perf log lines ([Options] PerfLog=yes or the perflog
// autotest step) or the performance overlay. While it is off, a scope costs one flag
// test. With the log on, once a second the log gets Perf lines with the average, 95th
// percentile and worst time of each phase.
//
// A frame phase has one sample per game frame, a draw phase one per draw (one call
// to GScreenClass::Render) and a present phase one per present call that reaches the
// presenter. A phase nested in another phase is also counted in the outer one.

#pragma once

#include "perfring.h"

#include <cstdint>
#include <vector>

enum PerfPhase : unsigned char {
	PERF_FRAME_LOGIC,           // Logic.AI() for one game frame
	PERF_FRAME_TRIGGERS,        // the tag triggers that LogicClass::AI springs each frame
	PERF_FRAME_TEAMS,           // team AI
	PERF_FRAME_OBJECTS,         // object AI, all kinds; contains the four kinds below
	PERF_FRAME_AI_INFANTRY,     // infantry AI
	PERF_FRAME_AI_UNIT,         // vehicle AI
	PERF_FRAME_AI_BUILDING,     // structure AI
	PERF_FRAME_AI_AIRCRAFT,     // aircraft AI
	PERF_FRAME_AI_OTHER,        // bullets, animations, particles and other objects
	PERF_FRAME_PATHFIND,        // AStarClass::Find_Path; nested in the object AI
	PERF_FRAME_THREAT,          // Greatest_Threat target scans; nested in the object AI
	PERF_FRAME_THREAT_RING,     // cell ring walk of an area or range scan; nested in THREAT, contains THREAT_EVAL
	PERF_FRAME_THREAT_EVAL,     // Evaluate_Object calls from the ring walk; nested in THREAT_RING
	PERF_FRAME_MAP,             // Map.Logic()
	PERF_FRAME_TACTICAL,        // TacticalMap->AI()
	PERF_FRAME_FACTORIES,       // factory AI
	PERF_FRAME_HOUSES,          // house AI, including the computer houses
	PERF_FRAME_QUEUE,           // Queue_AI(): player and network orders
	PERF_FRAME_DETACH,          // Detach_This_From_All; nested in whatever removed the object
	PERF_FRAME_WAIT,            // sleeping between game frames in Sync_Delay
	PERF_DRAW_TOTAL,            // one GScreenClass::Render, including any present it makes
	PERF_DRAW_TERRAIN,          // rebuild of the cached terrain pass, when the view needs one
	PERF_DRAW_SHROUD,           // shroud layer; nested in DRAW_TERRAIN
	PERF_DRAW_TILECOPY,         // copy of the cached terrain into the frame
	PERF_DRAW_OBJECTS,          // Tactical::Draw_Objects
	PERF_DRAW_VOXELS,           // voxel bodies and turrets; nested in DRAW_OBJECTS
	PERF_DRAW_PARTICLES,        // particles; nested in DRAW_OBJECTS
	PERF_DRAW_FX,               // lasers, bolts, beams, line trails, spot lights, links and beacons
	PERF_DRAW_OVERLAY,          // orders, placement, rubber band, indicators and screen text
	PERF_DRAW_HUD,              // sidebar and radar (GScreenClass::Draw_It)
	PERF_DRAW_UI,               // panel backdrops, buttons, messages and tool tips
	PERF_DRAW_GPUMAP,           // GpuDraw_End: reading the map area and buffers and submitting the GPU map pass
	PERF_PRESENT_TOTAL,         // one present, from the presenter's start to its end
	PERF_PRESENT_UPLOAD,        // Backend_Present: texture uploads and the frame's passes
	PERF_PRESENT_GPU,           // Backend_End_Frame: bgfx::frame, which can wait for the GPU
	PERF_PRESENT_GPU_TIME,      // GPU time of the newest frame the GPU finished, as bgfx reports it
	PERF_PHASE_COUNT
};

enum PerfCounter : unsigned char {
	PERF_COUNT_OBJECTS,         // objects visited by Tactical::Draw_Objects, not counting terrain objects
	PERF_COUNT_VOXELS,          // voxel draws (bodies and turrets; not shadows)
	PERF_COUNT_VOXEL_HIT,       // voxel draws found in the cache, in a draw that is not between game frames
	PERF_COUNT_VOXEL_MISS,      // cache-eligible voxel draws that had to render, same kind of draw
	PERF_COUNT_VOXEL_SMOOTH_HIT,    // as VOXEL_HIT, in a draw between game frames
	PERF_COUNT_VOXEL_SMOOTH_MISS,   // as VOXEL_MISS, in a draw between game frames
	PERF_COUNT_VOXEL_UNCACHED,  // voxel draws that do not use the cache
	PERF_COUNT_PARTICLES,       // particle draws
	PERF_COUNT_PATHS,           // path searches (AStarClass::Find_Path)
	PERF_COUNT_THREATS,         // target scans (TechnoClass::Greatest_Threat)
	PERF_COUNT_OBJECT_AI,       // object AI calls
	PERF_COUNT_UPLOAD_BYTES,    // bytes passed to texture uploads
	PERF_COUNT_UPLOADS,         // texture uploads
	PERF_COUNT_SCANS_RING,      // target scans that walk the ring of cells (area and range scans)
	PERF_COUNT_SCANS_AIR,       // target scans that walk the aircraft and air layer lists (air scans)
	PERF_COUNT_SCANS_FULL,      // target scans of the full map (aircraft list and Technos)
	PERF_COUNT_SCAN_TARGETING,  // Greatest_Threat calls from Target_Something_Nearby
	PERF_COUNT_SCAN_MISSION,    // Greatest_Threat calls from foot area scans (attack move, patrol, guard area)
	PERF_COUNT_SCAN_TEAM,       // Greatest_Threat calls from team AI
	PERF_COUNT_SCAN_HOUSE,      // Greatest_Threat calls from house AI (quarry)
	PERF_COUNT_RING_CELLS,      // ring cells on the radar map that the walk visits
	PERF_COUNT_CELL_CANDIDATES, // Evaluate_Cell calls that found an occupant to evaluate
	PERF_COUNT_OBJECT_EVALS,    // Evaluate_Object calls
	PERF_COUNT_AREA_MODIFY,     // Area_Modify calls
	PERF_COUNT_WEAPON_SELECTS,  // What_Weapon_Should_I_Use calls
	PERF_COUNT_DETACH_CALLS,    // Detach_This_From_All calls
	PERF_COUNT_DETACH_VISITS,   // Detach calls made by those broadcasts
	PERF_COUNT_DETACH_BULLET,   // broadcasts for a bullet
	PERF_COUNT_DETACH_ANIM,     // broadcasts for an animation
	PERF_COUNT_DETACH_PARTICLE, // broadcasts for a particle or particle system
	PERF_COUNT_DETACH_TECHNO,   // broadcasts for a unit, infantry, aircraft or building
	PERF_COUNT_DETACH_OTHER,    // all other broadcasts
	PERF_COUNT_DRAWS_SKIPPED,   // draw slots in the frame wait that came due with too little time left; once per slot
	PERF_COUNT_PRESENTS_BUSY,   // presents put off because the render thread had not finished the last frame
	PERF_COUNT_GPU_VOXEL_PARTS, // voxel parts of the passes the GPU drew
	PERF_COUNT_GPU_VOXEL_DIRECT, // those parts a blit shows
	PERF_COUNT_GPU_VOXEL_FACES, // cube faces those parts' draws submitted (a pass scale above 1)
	PERF_COUNT_LOGIC_OVER_20,   // game frames whose logic took over 20 ms
	PERF_COUNT_LOGIC_OVER_33,   // game frames whose logic took over 33 ms
	PERF_COUNT_LOGIC_OVER_50,   // game frames whose logic took over 50 ms
	PERF_COUNT_PRESENTS_HELD,   // presents whose hand-over to the renderer took over a quarter of the present interval
	PERF_COUNTER_COUNT
};

enum PerfDomain : unsigned char {
	PERF_DOMAIN_FRAME,
	PERF_DOMAIN_DRAW,
	PERF_DOMAIN_PRESENT,
	PERF_DOMAIN_COUNT
};

// What wants recording on. Recording runs while any user is on.
enum PerfUser : unsigned char {
	PERF_USER_LOG = 1,          // the once-a-second Perf lines
	PERF_USER_OVERLAY = 2,      // the performance overlay
};

// A phase's name in the Perf lines and the phase it runs inside, PERF_PHASE_COUNT for none. A phase
// with Across set runs inside several rows of its parent, so it is part of whichever one ran it.
struct PerfPhaseInfo {
	char const * Name;
	PerfPhase Parent;
	bool Across;
};

// One row of a domain's breakdown: a phase and how many parents it has.
struct PerfPhaseRow {
	PerfPhase Phase;
	int Depth;
};

extern bool Perf_Enabled;

// Set on a thread other than the main one to keep its counts out of the unlocked totals.
extern thread_local bool Perf_Thread_Muted;

// Turns one user of recording on or off. Recording turned on clears the counts and starts a new
// second; the log user turned on starts a new second of Perf lines.
void Perf_Set_User(PerfUser user, bool on);
bool Perf_Has_User(PerfUser user);

PerfPhaseInfo const & Perf_Phase_Info(PerfPhase phase);
PerfDomain Perf_Phase_Domain(PerfPhase phase);

// The phases of a domain, each after its parent and its parent's earlier children, in PerfPhase order
// among siblings.
std::vector<PerfPhaseRow> Perf_Phase_Rows(PerfDomain domain);

// The recent samples while recording, for the overlay. A phase's ring holds one value per sample of
// its domain in milliseconds, and the domain's ring of times holds when each sample was taken, as
// Monotonic_Nanoseconds() / 1e6; the newest entries of the two match.
PerfRing<float> const & Perf_Phase_History(PerfPhase phase);
PerfRing<double> const & Perf_Domain_Times(PerfDomain domain);

// Milliseconds from the end of one game frame to the end of the next, and from the start of one
// present to the start of the next.
PerfRing<float> const & Perf_Frame_Intervals(void);
PerfRing<float> const & Perf_Present_Gaps(void);

// A counter's total over the last whole second of recording.
std::uint64_t Perf_Last_Second_Count(PerfCounter counter);

void Perf_Add_Count(PerfCounter counter, std::uint64_t amount);
void Perf_Begin(PerfPhase phase);
void Perf_End(PerfPhase phase);

// Adds a time measured elsewhere to a phase of the current sample while recording is on.
void Perf_Add_Time(PerfPhase phase, double milliseconds);

// Adds to a counter while recording is on.
inline void Perf_Count(PerfCounter counter, std::uint64_t amount = 1)
{
	if (Perf_Enabled) {
		Perf_Add_Count(counter, amount);
	}
}

// Times a phase from its construction to the end of its scope. A phase that is already
// running in an outer scope adds nothing, so recursive calls are timed once.
class PerfScope
{
	public:
		explicit PerfScope(PerfPhase phase) : Phase(phase), Active(Perf_Enabled)
		{
			if (Active) {
				Perf_Begin(Phase);
			}
		}

		~PerfScope()
		{
			if (Active) {
				Perf_End(Phase);
			}
		}

		PerfScope(PerfScope const &) = delete;
		PerfScope & operator=(PerfScope const &) = delete;

	private:
		PerfPhase Phase;
		bool Active;
};

// Ends a draw and records its sample of the draw phases.
void Perf_Draw_End(void);

// Starts a present: its gap from the previous present's start is sampled.
void Perf_Present_Begin(void);

// Ends a present and records its sample of the present phases.
void Perf_Present_End(void);

// Records one draw sample when it goes out of scope. Declare it before the PerfScope that
// times the whole draw, so that scope ends first and its time is in the sample.
class PerfDrawSample
{
	public:
		PerfDrawSample(void) = default;
		~PerfDrawSample()
		{
			if (Perf_Enabled) {
				Perf_Draw_End();
			}
		}

		PerfDrawSample(PerfDrawSample const &) = delete;
		PerfDrawSample & operator=(PerfDrawSample const &) = delete;
};

// Records one present sample when it goes out of scope; declared before the scope that
// times the present, as for PerfDrawSample.
class PerfPresentSample
{
	public:
		PerfPresentSample(void)
		{
			if (Perf_Enabled) {
				Perf_Present_Begin();
			}
		}
		~PerfPresentSample()
		{
			if (Perf_Enabled) {
				Perf_Present_End();
			}
		}

		PerfPresentSample(PerfPresentSample const &) = delete;
		PerfPresentSample & operator=(PerfPresentSample const &) = delete;
};

// Ends a game frame: pushes the frame samples and writes the Perf lines once a second.
void Perf_Frame_End(void);
