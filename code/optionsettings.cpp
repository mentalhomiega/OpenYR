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

constexpr NameEntry Weather_Names[] = {
	{ "None", WEATHER_NONE },
	{ "Map", WEATHER_MAP },
	{ "Rain", WEATHER_RAIN },
	{ "Snow", WEATHER_SNOW },
};


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
constexpr char const * GPU_DRAWING = "GPU drawing";
constexpr char const * GPU_VOXELS = "GPU voxels";
constexpr char const * VOXEL_DRAWING = "Voxel drawing";
constexpr char const * PERFORMANCE = "performance";
constexpr char const * RENDERING = "Rendering";
constexpr char const * PATH_SEARCHES = "Path searches";


// To add a page, add an entry here and name its Id in the .Page of its rows.
constexpr SettingPage Pages[] = {
	{ .Id = PERFORMANCE, .Label = "Performance" },
};


// The order is the order of the keys in a saved RA2MD.INI: a save rewrites each key in this order.
constexpr Setting<OptionsClass> Settings[] = {
	{
		.Section = "Video", .Key = "GpuRender",
		.Flag = &OptionsClass::GpuRender, .Default = true,
		.Page = GRAPHICS, .Group = GPU_DRAWING, .Label = "Draw units on the GPU",
		.Tip = "Draws units and effects with the graphics card instead of the processor. Needs the modern interface and the Direct3D 11 renderer.",
		.Condition = SettingCondition::MODERN_INTERFACE,
	},
	{
		.Section = "Video", .Key = "SoftShadows",
		.Flag = &OptionsClass::SoftShadows, .Default = false,
		.Page = GRAPHICS, .Group = GPU_DRAWING, .Label = "Soft shadows",
		.Tip = "Blends every shadow on the ground, including those of buildings and trees, into one soft shadow, so overlapping shadows do not darken twice. Needs GPU drawing.",
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "SoftShadowRadius",
		.Number = &OptionsClass::SoftShadowRadius, .Default = 3, .Min = 1, .Max = 16,
	},
	{
		.Section = "Video", .Key = "Shockwaves",
		.Flag = &OptionsClass::Shockwaves, .Default = false,
		.Page = GRAPHICS, .Group = GPU_DRAWING, .Label = "Shockwaves",
		.Tip = "Weapon blasts send a ripple through the picture around them. Needs GPU drawing.",
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "Bloom",
		.Flag = &OptionsClass::Bloom, .Default = false,
		.Page = GRAPHICS, .Group = GPU_DRAWING, .Label = "Glow",
		.Tip = "Lasers, lamps, fire and other bright effects glow onto the picture around them. Needs GPU drawing.",
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "BloomStrength",
		.Number = &OptionsClass::BloomStrength, .Default = 100, .Min = 1, .Max = 400,
	},
	{
		.Section = "Video", .Key = "WaterRipples",
		.Flag = &OptionsClass::WaterRipples, .Default = false,
		.Page = GRAPHICS, .Group = GPU_DRAWING, .Label = "Water ripples",
		.Tip = "Sea, lakes and rivers ripple and shimmer. Needs GPU drawing.",
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "WaterRippleStrength",
		.Number = &OptionsClass::WaterRippleStrength, .Default = 100, .Min = 1, .Max = 400,
	},
	{
		.Section = "Video", .Key = "CloudShadows",
		.Flag = &OptionsClass::CloudShadows, .Default = false,
		.Page = GRAPHICS, .Group = GPU_DRAWING, .Label = "Cloud shadows",
		.Tip = "The shadows of slowly drifting clouds pass over the battlefield. Needs GPU drawing.",
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "CloudShadowStrength",
		.Number = &OptionsClass::CloudShadowStrength, .Default = 100, .Min = 1, .Max = 400,
	},
	{
		.Section = "Video", .Key = "GpuVoxels",
		.Flag = &OptionsClass::GpuVoxels, .Default = false,
		.Page = GRAPHICS, .Group = GPU_VOXELS, .Label = "Draw voxels on the GPU",
		.Tip = "Draws vehicles and aircraft from their voxels with the graphics card, so turrets and barrels hide each other by depth. Experimental. Needs GPU drawing.",
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "GpuVoxelSmooth",
		.Flag = &OptionsClass::GpuVoxelSmooth, .Default = false,
		.Page = GRAPHICS, .Group = GPU_VOXELS, .Label = "Smooth voxel turning",
		.Tip = "An object drawn from its voxels turns to its exact facing instead of the nearest of 32. Works with Draw voxels on the GPU. Needs GPU drawing.",
		.Needs = "GpuRender",
	},
	{
		.Section = "Video", .Key = "GpuVoxelScale",
		.Number = &OptionsClass::GpuVoxelScale, .Default = 0, .Min = 0, .Max = GPU_VOXEL_SCALE_MAX,
		.Page = GRAPHICS, .Group = GPU_VOXELS, .Label = "Voxel sharpness",
		.Tip = "Draws the map at this many times its pixels, so voxel objects stay sharp in a zoomed view. Auto follows the zoom. Works with Draw voxels on the GPU. Needs GPU drawing.",
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
		.Page = GRAPHICS, .Group = VOXEL_DRAWING, .Label = "Steady voxel placement",
		.Tip = "A turning vehicle or aircraft stays in place instead of wobbling by a pixel. Off places it as the original game does.",
	},
	{
		.Section = "Video", .Key = "ShadowStacking",
		.Flag = &OptionsClass::ShadowStacking, .Default = true,
		.Page = GRAPHICS, .Group = VOXEL_DRAWING, .Label = "Stacked shadows",
		.Tip = "Overlapping vehicle and aircraft shadows darken the ground again under each, as in the original game. Off darkens it once, so a crowd of aircraft leaves a grey patch instead of a black one.",
	},
	{
		.Section = "Video", .Key = "Weather",
		.Number = &OptionsClass::Weather, .Default = WEATHER_NONE, .Names = Weather_Names,
		.Page = GRAPHICS, .Group = GPU_DRAWING, .Label = "Weather",
		.Tip = "Snow falls on snowy maps, and rain falls during a lightning storm. Needs GPU drawing.",
		.Needs = "GpuRender", .SwitchOn = WEATHER_MAP,
	},
	{
		.Section = "Video", .Key = "WeatherStrength",
		.Number = &OptionsClass::WeatherStrength, .Default = 100, .Min = 1, .Max = 400,
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


std::span<SettingPage const> Option_Setting_Pages(void)
{
	return(Pages);
}
