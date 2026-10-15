/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "hdrdisplay.h"

#include "dbgprint.h"

#include <cstdio>

#ifdef _WIN32
#include <dxgi1_6.h>
#include <windows.h>

#include <string>
#include <vector>
#endif


namespace
{

HDRDisplayInfo CurrentDisplay;

}


#ifdef _WIN32

/// <summary>
/// Reads the DXGI output on the monitor, with its peak luminance and HDR state.
/// </summary>
/// <returns>bool; Was an output on that monitor found?</returns>
static bool Read_DXGI_Output(HMONITOR monitor, HDRDisplayInfo & info, std::wstring & device)
{
	IDXGIFactory1 * factory = nullptr;
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&factory))) {
		return(false);
	}

	bool found = false;
	for (UINT adapterindex = 0; !found; adapterindex++) {
		IDXGIAdapter1 * adapter = nullptr;
		if (factory->EnumAdapters1(adapterindex, &adapter) == DXGI_ERROR_NOT_FOUND) {
			break;
		}

		for (UINT outputindex = 0; !found; outputindex++) {
			IDXGIOutput * output = nullptr;
			if (adapter->EnumOutputs(outputindex, &output) == DXGI_ERROR_NOT_FOUND) {
				break;
			}

			DXGI_OUTPUT_DESC desc;
			if (SUCCEEDED(output->GetDesc(&desc)) && desc.Monitor == monitor) {
				IDXGIOutput6 * output6 = nullptr;
				if (SUCCEEDED(output->QueryInterface(__uuidof(IDXGIOutput6), (void **)&output6))) {
					DXGI_OUTPUT_DESC1 desc1;
					if (SUCCEEDED(output6->GetDesc1(&desc1))) {
						info.Found = true;
						info.HDREnabled = (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
						info.PeakNits = desc1.MaxLuminance;
						info.FullFrameNits = desc1.MaxFullFrameLuminance;
						device = desc.DeviceName;
						found = true;
					}
					output6->Release();
				}
			}
			output->Release();
		}
		adapter->Release();
	}

	factory->Release();
	return(found);
}


/// <summary>
/// Reads the SDR content brightness Windows applies to the display with the given GDI name.
/// </summary>
static void Read_SDR_White(std::wstring const & device, HDRDisplayInfo & info)
{
	UINT32 pathcount = 0;
	UINT32 modecount = 0;
	if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathcount, &modecount) != ERROR_SUCCESS) {
		return;
	}

	std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathcount);
	std::vector<DISPLAYCONFIG_MODE_INFO> modes(modecount);
	if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathcount, paths.data(), &modecount, modes.data(), nullptr) != ERROR_SUCCESS) {
		return;
	}

	for (UINT32 index = 0; index < pathcount; index++) {
		DISPLAYCONFIG_SOURCE_DEVICE_NAME source = {};
		source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
		source.header.size = sizeof(source);
		source.header.adapterId = paths[index].sourceInfo.adapterId;
		source.header.id = paths[index].sourceInfo.id;
		if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS || device != source.viewGdiDeviceName) {
			continue;
		}

		DISPLAYCONFIG_SDR_WHITE_LEVEL white = {};
		white.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
		white.header.size = sizeof(white);
		white.header.adapterId = paths[index].targetInfo.adapterId;
		white.header.id = paths[index].targetInfo.id;
		if (DisplayConfigGetDeviceInfo(&white.header) == ERROR_SUCCESS) {
			// Windows counts the level in thousandths of 80 nits.
			info.SDRWhiteKnown = true;
			info.SDRWhiteNits = (float)white.SDRWhiteLevel * 80.0f / 1000.0f;
		}
		return;
	}
}

#endif


void HDR_Query_Window_Display(void * window)
{
	CurrentDisplay = HDRDisplayInfo();

#ifdef _WIN32
	if (window != nullptr) {
		HMONITOR monitor = MonitorFromWindow((HWND)window, MONITOR_DEFAULTTONEAREST);
		std::wstring device;
		if (Read_DXGI_Output(monitor, CurrentDisplay, device)) {
			Read_SDR_White(device, CurrentDisplay);
		}
	}
#endif

	if (!CurrentDisplay.Found) {
		DebugString("HDR: no display reported for the window's monitor\n");
		return;
	}

	char sdrwhite[32] = "unknown";
	if (CurrentDisplay.SDRWhiteKnown) {
		std::snprintf(sdrwhite, sizeof(sdrwhite), "%.0f nits", CurrentDisplay.SDRWhiteNits);
	}
	DebugString("HDR: Windows HDR %s, peak %.0f nits, full frame %.0f nits, SDR white %s\n",
		CurrentDisplay.HDREnabled ? "on" : "off", CurrentDisplay.PeakNits, CurrentDisplay.FullFrameNits, sdrwhite);
}


HDRDisplayInfo const & HDR_Current_Display(void)
{
	return(CurrentDisplay);
}
