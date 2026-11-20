/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// To add a client setting, copy the nearest entry and add its member to OptionsClass. See docs/SETTINGS.md.

#include "always.h"

#include "optionsettings.h"

#include "gpuvoxel.h"


namespace
{

constexpr NameEntry GpuVoxelLayerDepth_Names[] = {
	{ "Shared", GPU_VOXEL_LAYER_DEPTH_SHARED },
	{ "Ordered", GPU_VOXEL_LAYER_DEPTH_ORDERED },
};

constexpr NameEntry GpuVoxelCloak_Names[] = {
	{ "Translucent", GPU_VOXEL_CLOAK_TRANSLUCENT },
	{ "Ripple", GPU_VOXEL_CLOAK_RIPPLE },
};

constexpr NameEntry PerfOverlay_Names[] = {
	{ "Off", PERF_OVERLAY_OFF },
	{ "Basic", PERF_OVERLAY_BASIC },
	{ "Detailed", PERF_OVERLAY_DETAILED },
};


constexpr NameEntry ZoomOutView_Names[] = {
	{ "Original", ZOOM_OUT_VIEW_ORIGINAL },
	{ "4K", ZOOM_OUT_VIEW_4K },
};


constexpr char const * GRAPHICS = "graphics";
constexpr char const * QUALITY = "Graphics quality";
constexpr char const * ADVANCED = "Advanced";
constexpr char const * GPU_DRAWING = "Graphics card";
constexpr char const * GPU_VOXELS = "Vehicles and aircraft";
constexpr char const * VOXEL_DRAWING = "Vehicle details";
constexpr char const * STOCK_ART = "Stock art";
constexpr char const * PERFORMANCE = "performance";
constexpr char const * RENDERING = "Rendering";
constexpr char const * PATH_SEARCHES = "Path searches";


// Classic uses the classic sidebar and draws with the processor, as Yuri's Revenge does.
constexpr SettingPresetValue Classic_Values[] = {
	{ MODERN_INTERFACE_ROW, 0 }, { "GpuRender", 0 }, { "GpuVoxels", 0 }, { "GpuVoxelSmooth", 0 }, { "GpuVoxelSubpixel", 0 },
};

// The default: the modern interface and drawing with the graphics card, which leave the picture as it was.
constexpr SettingPresetValue Enhanced_Values[] = {
	{ MODERN_INTERFACE_ROW, 1 }, { "GpuRender", 1 }, { "GpuVoxels", 0 }, { "GpuVoxelSmooth", 0 }, { "GpuVoxelSubpixel", 0 },
};

// Enhanced, with vehicles and aircraft drawn in 3D, turning and moving smoothly, at four times the map's pixels.
constexpr SettingPresetValue Smooth_Values[] = {
	{ MODERN_INTERFACE_ROW, 1 }, { "GpuRender", 1 }, { "GpuVoxels", 1 }, { "GpuVoxelSmooth", 1 }, { "GpuVoxelSubpixel", 1 },
	{ "GpuVoxelScale", 4 },
};

constexpr SettingPreset Graphics_Presets[] = {
	{ .Label = "Classic", .Values = Classic_Values },
	{ .Label = "Enhanced", .Values = Enhanced_Values },
	{ .Label = "Smooth", .Values = Smooth_Values },
};

// The choice shows the preset that the rows it names add up to, and Custom for any other mix.
constexpr SettingPresetChoice Presets[] = {
	{
		.Page = GRAPHICS, .Key = "GraphicsQuality", .Group = QUALITY, .Label = "Preset",
		.Tip = "Classic uses the classic sidebar and draws with the processor, as the original game does. Enhanced, the default, adds the modern interface and draws with the graphics card, which leaves the picture as it was. The third preset also draws vehicles and aircraft in 3D, turning and moving smoothly and sharp at every zoom. Custom means the switches match none of them.",
		.Presets = Graphics_Presets,
	},
};


// To add a page, add an entry here and name its Id in the .Page of its rows.
constexpr SettingPage Pages[] = {
	{ .Id = PERFORMANCE, .Label = "Performance" },
};


// The order is the order of the keys in a saved RA2MD.INI: a save rewrites each key in this order.
constexpr Setting<OptionsClass> Settings[] = {
	{
		.Section = "Video", .Key = "GpuRender",
		.Flag = &OptionsClass::GpuRender, .Default = true,
		.Page = GRAPHICS, .Group = GPU_DRAWING, .Label = "Draw with the graphics card",
		.Tip = "Draws units and effects with the graphics card instead of the processor, which keeps big battles smooth. The switches below need it. Needs the modern interface and the Direct3D 11 renderer.",
		.Fold = ADVANCED,
		.Condition = SettingCondition::MODERN_INTERFACE,
	},
	{
		.Section = "Video", .Key = "GpuVoxels",
		.Flag = &OptionsClass::GpuVoxels, .Default = false,
		.Page = GRAPHICS, .Group = GPU_VOXELS, .Label = "Draw vehicles and aircraft in 3D",
		.Tip = "Draws vehicles and aircraft from their 3D models with the graphics card, so turrets and barrels hide each other by depth. The next three rows work with it. Needs drawing with the graphics card.",
		.Fold = ADVANCED,
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "GpuVoxelSmooth",
		.Flag = &OptionsClass::GpuVoxelSmooth, .Default = false,
		.Page = GRAPHICS, .Group = GPU_VOXELS, .Label = "Smooth turning",
		.Tip = "A vehicle or aircraft drawn in 3D turns to its exact facing instead of the nearest of 32. Works with Draw vehicles and aircraft in 3D. Needs drawing with the graphics card.",
		.Fold = ADVANCED,
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "GpuVoxelSubpixel",
		.Flag = &OptionsClass::GpuVoxelSubpixel, .Default = false,
		.Page = GRAPHICS, .Group = GPU_VOXELS, .Label = "Smooth movement when zoomed in",
		.Tip = "In a zoomed-in view, a moving or hovering vehicle or aircraft drawn in 3D is placed by screen pixels instead of map pixels, so it glides instead of stepping. Works with Draw vehicles and aircraft in 3D. Needs drawing with the graphics card.",
		.Fold = ADVANCED,
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "GpuVoxelScale",
		.Number = &OptionsClass::GpuVoxelScale, .Default = 0, .Min = 0, .Max = GPU_VOXEL_SCALE_MAX,
		.Page = GRAPHICS, .Group = GPU_VOXELS, .Label = "Vehicle sharpness",
		.Tip = "Draws the map at this many times its pixels, so vehicles and aircraft drawn in 3D stay sharp in a zoomed view. Auto follows the zoom. Works with Draw vehicles and aircraft in 3D. Needs drawing with the graphics card.",
		.Fold = ADVANCED,
		.Needs = "GpuRender", .Unit = "x", .ZeroText = "Auto",
	},
	{
		.Section = "Video", .Key = "GpuVoxelLayerDepth",
		.Number = &OptionsClass::GpuVoxelLayerDepth, .Default = GPU_VOXEL_LAYER_DEPTH_SHARED, .Names = GpuVoxelLayerDepth_Names,
	},
	{
		.Section = "Video", .Key = "GpuVoxelCloak",
		.Number = &OptionsClass::GpuVoxelCloak, .Default = GPU_VOXEL_CLOAK_TRANSLUCENT, .Names = GpuVoxelCloak_Names,
	},
	{
		.Section = "Video", .Key = "SteadyVoxelPlacement",
		.Flag = &OptionsClass::SteadyVoxelPlacement, .Default = true,
		.Page = GRAPHICS, .Group = VOXEL_DRAWING, .Label = "Steady vehicle placement",
		.Tip = "A turning vehicle or aircraft stays in place instead of wobbling by a pixel. Off places it as the original game does.",
		.Fold = ADVANCED,
	},
	{
		.Section = "Video", .Key = "ShadowStacking",
		.Flag = &OptionsClass::ShadowStacking, .Default = true,
		.Page = GRAPHICS, .Group = VOXEL_DRAWING, .Label = "Darker overlapping shadows",
		.Tip = "Overlapping vehicle and aircraft shadows darken the ground again under each, as in the original game. Off darkens it once, so a crowd of aircraft leaves a grey patch instead of a black one.",
		.Fold = ADVANCED,
	},
	{
		.Section = "Video", .Key = "StockArtFixes",
		.Flag = &OptionsClass::StockArtFixes, .Default = true,
		.Page = GRAPHICS, .Group = STOCK_ART, .Label = "Fix stock art",
		.Tip = "Draws stock art that the original game draws wrongly the way it was meant to look, such as the grey apartment blocks, which the original shows as speckled noise. Off draws it as the original game does.",
		.NeedsRestart = true,
	},
	{
		.Section = "Video", .Key = "RenderThread",
		.Flag = &OptionsClass::RenderThread, .Default = false,
		.Page = PERFORMANCE, .Group = RENDERING, .Label = "Render on a separate thread",
		.Tip = "Hands the graphics card's work and the showing of each picture to a thread of their own, so a slow picture holds up the game less. Works with the automatic or Direct3D 11 graphics API; others ignore it.",
		.NeedsRestart = true, .BeforeRenderer = true,
	},
	{
		.Section = "Video", .Key = "FastestGpu",
		.Flag = &OptionsClass::FastestGpu, .Default = true,
		.Page = PERFORMANCE, .Group = RENDERING, .Label = "Use the fastest graphics card",
		.Tip = "On a computer with two graphics cards, draws on the one Windows ranks fastest. Off draws on the first one Windows lists, which is often the slower one built into the processor.",
		.NeedsRestart = true, .BeforeRenderer = true,
	},
	{
		.Section = "Options", .Key = "ParallelPaths",
		.Flag = &OptionsClass::ParallelPaths, .Default = false,
		.Page = PERFORMANCE, .Group = PATH_SEARCHES, .Label = "Parallel path searches",
		.Tip = "Searches the paths of infantry and vehicles on several processor threads, which shortens the longest frames in large battles. Paths can differ slightly from the original game's. Applies to campaign and skirmish games started after the change.",
	},
	{
		.Section = "Options", .Key = "PathThreads",
		.Number = &OptionsClass::PathThreads, .Default = 0, .Min = 0, .Max = 32,
		.Page = PERFORMANCE, .Group = PATH_SEARCHES, .Label = "Path search threads",
		.Tip = "How many processor threads search paths. Auto leaves one thread free and uses at most 8. The number does not change any path.",
		.Needs = "ParallelPaths", .ZeroText = "Auto",
	},
	{
		.Section = "Video", .Key = "PerfOverlay",
		.Number = &OptionsClass::PerfOverlay, .Default = PERF_OVERLAY_OFF, .Names = PerfOverlay_Names,
		.Page = GRAPHICS, .Group = "Diagnostics", .Label = "Performance overlay",
		.Tip = "Shows the frame rates, a graph of recent frame times and where the time goes, over the top left of the map. The PerfOverlay key (Ctrl+Shift+P) also cycles it through Basic, Detailed and off.",
		.SwitchOn = PERF_OVERLAY_BASIC,
	},
	{
		.Section = "Video", .Key = "ZoomOutView",
		.Number = &OptionsClass::ZoomOutView, .Default = ZOOM_OUT_VIEW_ORIGINAL, .Names = ZoomOutView_Names,
		.Page = GRAPHICS, .Group = "Camera", .Label = "Zoom out to a 4K view",
		.Tip = "Lets the mouse wheel zoom out until the view shows what the original game shows at 3840x2160, the most the game allows. Off stops at what it shows at 2560x1440. A screen smaller than the view has no more range than before.",
		.SwitchOn = ZOOM_OUT_VIEW_4K,
	},
	{
		// No row of its own: the game controls dialog shows it beside the Game Speed slider.
		.Section = "Options", .Key = "ClassicGameSpeeds",
		.Flag = &OptionsClass::ClassicGameSpeeds, .Default = false,
	},
};

}


std::span<Setting<OptionsClass> const> Option_Settings(void)
{
	return(Settings);
}


std::span<SettingPresetChoice const> Option_Setting_Presets(void)
{
	return(Presets);
}


std::span<SettingPage const> Option_Setting_Pages(void)
{
	return(Pages);
}
