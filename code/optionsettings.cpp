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


namespace
{

constexpr NameEntry Weather_Names[] = {
	{ "None", WEATHER_NONE },
	{ "Map", WEATHER_MAP },
	{ "Rain", WEATHER_RAIN },
	{ "Snow", WEATHER_SNOW },
};


constexpr char const * GRAPHICS = "graphics";
constexpr char const * GPU_DRAWING = "GPU drawing";
constexpr char const * GPU_VOXELS = "GPU voxels";


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
		.Tip = "Draws vehicles and aircraft from their voxels with the graphics card. Experimental. Nothing is drawn this way while Glow or HDR highlights are on. Needs GPU drawing.",
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
		.Number = &OptionsClass::GpuVoxelScale, .Default = 1, .Min = 1, .Max = 4,
		.Page = GRAPHICS, .Group = GPU_VOXELS, .Label = "Voxel sharpness",
		.Tip = "Draws the map at this many times its pixels, so voxel objects stay sharp in a zoomed view. Works with Draw voxels on the GPU. Needs GPU drawing.",
		.Needs = "GpuRender", .Unit = "x",
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
		.Flag = &OptionsClass::RenderThread, .Default = false, .BeforeRenderer = true,
	},
};

}


std::span<Setting<OptionsClass> const> Option_Settings(void)
{
	return(Settings);
}
