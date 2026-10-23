/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Timing and counts for the game frame, the draw and the present. Recording is off
// unless [Options] PerfLog=yes or the perflog autotest step turns it on; while it is
// off, a scope costs one flag test. Once a second the log gets Perf lines with the
// average, 95th percentile and worst time of each phase.
//
// A frame phase has one sample per game frame, a draw phase one per draw (one call
// to GScreenClass::Render) and a present phase one per present call that reaches the
// presenter. A phase nested in another phase is also counted in the outer one.

#pragma once

#include <cstdint>

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
	PERF_FRAME_MAP,             // Map.Logic()
	PERF_FRAME_TACTICAL,        // TacticalMap->AI()
	PERF_FRAME_FACTORIES,       // factory AI
	PERF_FRAME_HOUSES,          // house AI, including the computer houses
	PERF_FRAME_QUEUE,           // Queue_AI(): player and network orders
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
	PERF_PRESENT_TOTAL,         // one present, from the presenter's start to its end
	PERF_PRESENT_UPLOAD,        // Backend_Present: texture uploads and the frame's passes
	PERF_PRESENT_GPU,           // Backend_End_Frame: bgfx::frame, which can wait for the GPU
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
	PERF_COUNTER_COUNT
};

extern bool Perf_Enabled;

// Turns recording on or off. Turning it on clears the counts and starts a new second.
void Perf_Set_Enabled(bool enabled);

void Perf_Add_Count(PerfCounter counter, std::uint64_t amount);
void Perf_Begin(PerfPhase phase);
void Perf_End(PerfPhase phase);

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
		PerfPresentSample(void) = default;
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
