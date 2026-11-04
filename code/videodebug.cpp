/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The autotest logs for presents and map draws: the presentdigest, drawdigest and drawprobe steps.
// The presenter calls only Video_Log_Present_Digest.

#include "always.h"

#include "video.h"

#include "_surface.h"
#include "blit.h"
#include "bgfxbackend.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "fnv.h"
#include "globals.h"
#include "surface.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>


#ifdef _WIN32
#include <windows.h>
#endif


// The presentdigest autotest step: when on, each present logs a hash of the frame and map layer it sends.
static bool _PresentDigest = false;
static std::uint64_t _PresentDigestCount = 0;

// The drawdigest autotest step: when on, each draw of the map logs a hash of the map view it drew.
static bool _DrawDigest = false;
static std::uint64_t _DrawDigestCount = 0;


// FNV-1a with 64-bit lanes: each whole 8-byte word of a row is mixed in, then the row's tail bytes one by one.
static std::uint64_t Digest_Rows(std::uint64_t hash, char const * base, int pitch, int row_bytes, int rows)
{
	for (int row = 0; row < rows; row++) {
		char const * line = base + (std::ptrdiff_t)row * pitch;
		int offset = 0;
		for (; offset + 8 <= row_bytes; offset += 8) {
			std::uint64_t word;
			std::memcpy(&word, line + offset, sizeof(word));
			hash ^= word;
			hash *= FNV64_PRIME;
		}
		for (; offset < row_bytes; offset++) {
			hash ^= (unsigned char)line[offset];
			hash *= FNV64_PRIME;
		}
	}
	return(hash);
}


// Logs the digest of the frame, and of the map layer when one is drawn under it, as the presenter is about to send them.
void Video_Log_Present_Digest(DSurface const * surface, BackendLayer const * layer)
{
	if (!_PresentDigest) {
		return;
	}
	char const * frame = (char const *)surface->Get_Buffer();
	unsigned long long frame_hash = 0;
	if (frame != NULL) {
		frame_hash = Digest_Rows(FNV64_BASIS, frame, surface->Stride(), surface->Get_Width() * surface->Bytes_Per_Pixel(), surface->Get_Height());
	}

	char layer_text[24] = "none";
	if (layer != NULL && layer->Pixels != NULL) {
		snprintf(layer_text, sizeof(layer_text), "%016llx", (unsigned long long)Digest_Rows(FNV64_BASIS, (char const *)layer->Pixels, layer->Pitch, layer->Width * 2, layer->Height));
	}

	DebugString("PRESENT_DIGEST frame %d present %llu hash %016llx layer %s\n", Frame, (unsigned long long)_PresentDigestCount, frame_hash, layer_text);
	_PresentDigestCount++;
}


/// <summary>
/// Turns the per-present PRESENT_DIGEST log on or off. Turning it on restarts the present index at 0.
/// </summary>
void Video_Set_Present_Digest(bool enabled)
{
	_PresentDigest = enabled;
	_PresentDigestCount = 0;
}


/// <summary>
/// Turns the per-draw DRAW_DIGEST log on or off. Turning it on restarts the draw index at 0.
/// </summary>
void Video_Set_Draw_Digest(bool enabled)
{
	_DrawDigest = enabled;
	_DrawDigestCount = 0;
}


/// <summary>
/// Logs a DRAW_DIGEST line with a hash of the area of the surface, when the drawdigest step is on.
/// The surface must not be locked by the caller.
/// </summary>
void Video_Note_Draw_Digest(Surface const & surface, Rect const & area)
{
	if (!_DrawDigest) {
		return;
	}
	Rect const clipped = Intersect(area, surface.Get_Rect());
	unsigned long long hash = FNV64_BASIS;
	if (clipped.Width > 0 && clipped.Height > 0) {
		char const * base = (char const *)surface.Lock(Point2D(clipped.X, clipped.Y));
		if (base != NULL) {
			hash = Digest_Rows(hash, base, surface.Stride(), clipped.Width * surface.Bytes_Per_Pixel(), clipped.Height);
			surface.Unlock();
		}
	}
	DebugString("DRAW_DIGEST frame %d draw %llu hash %016llx\n", Frame, (unsigned long long)_DrawDigestCount, hash);
	_DrawDigestCount++;
}


static bool _DrawProbe = false;
static Surface const * _DrawProbeTarget = NULL;
static std::map<std::vector<std::uintptr_t>, std::uint64_t> _DrawProbeStacks;
static std::map<std::string, std::pair<std::uint64_t, std::uint64_t>> _DrawProbeBlits;


/// <summary>
/// Turns the drawprobe autotest step on or off. Turning it off logs a DRAW_PROBE line for each call
/// stack that locked the map view during the foreground pass, with its count, most frequent first.
/// </summary>
void Video_Set_Draw_Probe(bool enabled)
{
	if (!enabled && _DrawProbe) {
		std::vector<std::pair<std::uint64_t, std::vector<std::uintptr_t> const *>> sorted;
		for (auto const & entry : _DrawProbeStacks) {
			sorted.emplace_back(entry.second, &entry.first);
		}
		std::sort(sorted.begin(), sorted.end(), [](auto const & a, auto const & b) { return a.first > b.first; });
		for (auto const & entry : sorted) {
			char text[256];
			int length = snprintf(text, sizeof(text), "DRAW_PROBE count %llu stack", (unsigned long long)entry.first);
			for (std::uintptr_t rva : *entry.second) {
				length += snprintf(text + length, sizeof(text) - length, " %08x", (unsigned)rva);
			}
			DebugString("%s\n", text);
		}
		_DrawProbeStacks.clear();
		for (auto const & entry : _DrawProbeBlits) {
			DebugString("DRAW_PROBE_BLIT count %llu pixels %llu kind %s\n", (unsigned long long)entry.second.first, (unsigned long long)entry.second.second, entry.first.c_str());
		}
		_DrawProbeBlits.clear();
	}
	_DrawProbe = enabled;
	Blit_Probe_Hook = enabled ? Video_Draw_Probe_Note_Blit : NULL;
}


/// <summary>
/// Names the surface whose locks the drawprobe step counts; NULL stops counting.
/// </summary>
void Video_Draw_Probe_Target(Surface const * surface)
{
	_DrawProbeTarget = _DrawProbe ? surface : NULL;
}


/// <summary>
/// Counts a blit into the surface by its blitter kind and clipped pixel area while the surface is the probe target.
/// </summary>
void Video_Draw_Probe_Note_Blit(Surface const * surface, char const * kind, int pixels)
{
	if (surface != _DrawProbeTarget || surface == NULL) {
		return;
	}
	auto & entry = _DrawProbeBlits[kind];
	entry.first++;
	entry.second += pixels;
}


/// <summary>
/// Counts a lock of the surface under its caller's stack while the surface is the probe target.
/// </summary>
void Video_Draw_Probe_Note_Lock(Surface const * surface)
{
	if (surface != _DrawProbeTarget || surface == NULL) {
		return;
	}
#ifdef _WIN32
	void * frames[6];
	USHORT const count = CaptureStackBackTrace(2, 6, frames, NULL);
	std::uintptr_t const base = (std::uintptr_t)GetModuleHandle(NULL);
	std::vector<std::uintptr_t> key;
	for (USHORT i = 0; i < count; i++) {
		key.push_back((std::uintptr_t)frames[i] - base);
	}
	_DrawProbeStacks[key]++;
#endif
}
