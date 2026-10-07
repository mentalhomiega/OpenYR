/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "binkmovie.h"

#include "_keyboar.h"
#include "_rect.h"
#include "_surface.h"
#include "_xmouse.h"
#include "audio/audioengine.h"
#include "ccfile.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "gamedirs.h"
#include "globals.h"
#include "goptions.h"
#include "gscreen.h"
#include "movieskip.h"
#include "sdl/sdlwindow.h"
#include "surface.h"
#include "ui/uiscript.h"
#include "video.h"
#include "win.h"

#include <algorithm>
#include <string>
#include <vector>

#ifdef _WIN32

namespace
{

// The head of Bink's HBINK structure, which every Bink 1.x release lays out alike.
struct BinkInfoType
{
	unsigned Width;
	unsigned Height;
	unsigned Frames;
	unsigned FrameNum;
	unsigned LastFrameNum;
	unsigned FrameRate;
	unsigned FrameRateDiv;
};

constexpr unsigned BINK_FROM_MEMORY = 0x04000000;
constexpr unsigned BINK_SURFACE_555 = 9;
constexpr unsigned BINK_SURFACE_565 = 10;

typedef void * (__stdcall * BinkOpenFn)(char const * name, unsigned flags);
typedef void (__stdcall * BinkCloseFn)(void * bink);
typedef int (__stdcall * BinkStepFn)(void * bink);
typedef int (__stdcall * BinkCopyFn)(void * bink, void * dest, int pitch, unsigned height, unsigned x, unsigned y, unsigned flags);
typedef int (__stdcall * BinkSoundSystemFn)(void * open, unsigned param);
typedef int (__stdcall * BinkVolumeFn)(void * bink, int volume);
typedef int (__stdcall * BinkPauseFn)(void * bink, int pause);

struct BinkApiType
{
	HMODULE Module = nullptr;
	bool Tried = false;
	bool Ready = false;
	BinkOpenFn Open = nullptr;
	BinkCloseFn Close = nullptr;
	BinkStepFn DoFrame = nullptr;
	BinkStepFn NextFrame = nullptr;
	BinkStepFn Wait = nullptr;
	BinkCopyFn CopyToBuffer = nullptr;
	BinkSoundSystemFn SetSoundSystem = nullptr;
	void * OpenDirectSound = nullptr;
	BinkVolumeFn SetVolume = nullptr;
	BinkPauseFn Pause = nullptr;
};

BinkApiType Api;
bool Playing = false;

template<typename T> bool Resolve(T & target, char const * name)
{
	target = reinterpret_cast<T>(GetProcAddress(Api.Module, name));
	return(target != nullptr);
}

bool Load_Api(void)
{
	if (Api.Tried) {
		return(Api.Ready);
	}
	Api.Tried = true;

	std::string const beside = Data_Directory() + "BINKW32.DLL";
	Api.Module = LoadLibraryA(beside.c_str());
	if (Api.Module == nullptr) {
		Api.Module = LoadLibraryA("BINKW32.DLL");
	}
	if (Api.Module == nullptr) {
		DebugString("BINK BINKW32.DLL not found; Bink movies cannot play\n");
		return(false);
	}

	Api.Ready = Resolve(Api.Open, "_BinkOpen@8")
		&& Resolve(Api.Close, "_BinkClose@4")
		&& Resolve(Api.DoFrame, "_BinkDoFrame@4")
		&& Resolve(Api.NextFrame, "_BinkNextFrame@4")
		&& Resolve(Api.Wait, "_BinkWait@4")
		&& Resolve(Api.CopyToBuffer, "_BinkCopyToBuffer@28")
		&& Resolve(Api.SetSoundSystem, "_BinkSetSoundSystem@8")
		&& Resolve(Api.OpenDirectSound, "_BinkOpenDirectSound@4")
		&& Resolve(Api.SetVolume, "_BinkSetVolume@8")
		&& Resolve(Api.Pause, "_BinkPause@8");
	if (!Api.Ready) {
		DebugString("BINK BINKW32.DLL lacks the functions the movie player needs\n");
	}
	return(Api.Ready);
}

}


bool Bink_Is_Playing(void)
{
	return(Playing);
}


/// <summary>
/// Plays a Bink (.BIK) movie from the game's files, returning when it ends or the player skips it.
/// The movie is shown the way the VQA movies are: centered, or stretched to the screen when movie
/// stretching is on. It plays silently while the engine's master volume is zero, as in a scripted test.
/// </summary>
bool Bink_Play(char const * name)
{
	if (Playing || !Load_Api()) {
		return(false);
	}

	std::vector<char> data;
	{
		CCFileClass file(name);
		if (!file.Is_Available()) {
			return(false);
		}
		int const size = file.Size();
		if (size <= 0) {
			return(false);
		}
		data.resize((std::size_t)size);
		if (file.Read(data.data(), size) != size) {
			DebugString("BINK %s: could not read the whole movie\n", name);
			return(false);
		}
	}

	bool const sound = AudioEngine.Master_Gain() > 0.0f;
	if (sound) {
		Api.SetSoundSystem(Api.OpenDirectSound, 0);
	}

	void * bink = Api.Open(data.data(), BINK_FROM_MEMORY);
	if (bink == nullptr) {
		DebugString("BINK %s: not a movie Bink can open\n", name);
		return(false);
	}

	BinkInfoType const * info = static_cast<BinkInfoType const *>(bink);
	int const width = (int)info->Width;
	int const height = (int)info->Height;
	DebugString("BINK %s: %dx%d, %u frames at %u/%u\n", name, width, height, info->Frames, info->FrameRate, info->FrameRateDiv);
	Api.SetVolume(bink, sound ? (int)(32768.0f * AudioEngine.Master_Gain()) : 0);

	Surface * draw = HiddenSurface;
	draw->Fill(0);
	Update_Visible_Surface(HiddenSurface);

	Rect const source(0, 0, std::min(width, draw->Get_Width()), std::min(height, draw->Get_Height()));
	Rect area((VisibleRect.Width - width) / 2, (VisibleRect.Height - height) / 2, width, height);
	if (DSurface::AllowStretchBlits && Options.StretchMovies) {
		double const scale = std::min((double)VisibleRect.Width / width, (double)VisibleRect.Height / height);
		area.Width = (int)(width * scale);
		area.Height = (int)(height * scale);
		area.X = (VisibleRect.Width - area.Width) / 2;
		area.Y = (VisibleRect.Height - area.Height) / 2;
		DebugString("Stretching movie %dx%d -> %dx%d\n", width, height, area.Width, area.Height);
	}

	unsigned const format = (DSurface::Get_Primary_Color_Mode() == COLORMODE_555) ? BINK_SURFACE_555 : BINK_SURFACE_565;

	Playing = true;
	Hide_Mouse();
	Keyboard->Clear();

	bool paused = false;
	while (true) {
		Main_Window_Pump_Events();
		if (MovieSkip::Idle() || UIScript_Fullscreen_Tick()) {
			break;
		}

		if (!GameInFocus) {
			if (!paused) {
				Api.Pause(bink, 1);
				paused = true;
			}
			Sleep(33);
			continue;
		}
		if (paused) {
			Api.Pause(bink, 0);
			paused = false;
		}

		if (Api.Wait(bink) != 0) {
			Sleep(1);
			continue;
		}

		Api.DoFrame(bink);
		void * pixels = draw->Lock();
		if (pixels != nullptr) {
			Api.CopyToBuffer(bink, pixels, draw->Stride(), (unsigned)draw->Get_Height(), 0, 0, format);
			draw->Unlock();
		}

		VisibleSurface->Blit_From(area, *draw, source);
		MovieSkip::Draw_Overlay(*VisibleSurface, area);
		Video_Present_If_Dirty();

		if (info->FrameNum + 1 >= info->Frames) {
			break;
		}
		Api.NextFrame(bink);
	}

	Api.Close(bink);
	Playing = false;
	Show_Mouse();
	Keyboard->Clear();
	return(true);
}

#else

bool Bink_Is_Playing(void)
{
	return(false);
}


bool Bink_Play(char const *)
{
	return(false);
}

#endif
