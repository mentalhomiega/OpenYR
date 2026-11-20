/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins declaring, loading, saving, ranges, unknown keys and greying of the settings registry, and that
// the real table reads and writes a settings file exactly as the code it replaced did. Pass a settings
// file's path to round-trip that file as well; it is only read. Needs no game data.

#include "optionsettings.h"
#include "ui/uisettingrows.h"
#include "ini.h"
#include "xpipe.h"
#include "xstraw.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>


namespace {

int Failures = 0;
int Checked = 0;


void Check(bool condition, char const * what)
{
	Checked++;
	if (!condition) {
		Failures++;
		std::printf("FAIL: %s\n", what);
	}
}


struct Fields
{
	bool Master = false;
	bool Child = false;
	int Strength = 0;
	int Unbounded = 0;
	int Mode = 0;
	int Sharpness = 0;
};


constexpr NameEntry Mode_Names[] = {
	{ "Off", 0 },
	{ "Map", 1 },
	{ "Rain", 2 },
};


constexpr Setting<Fields> Table[] = {
	{ .Section = "Video", .Key = "Master", .Flag = &Fields::Master, .Default = true,
	  .Page = "test", .Group = "Group A", .Label = "Master", .Tip = "The master switch.",
	  .Condition = SettingCondition::MODERN_INTERFACE, .NeedsRestart = true },
	{ .Section = "Video", .Key = "ChildSwitch", .Flag = &Fields::Child, .Default = false,
	  .Page = "test", .Group = "Group A", .Label = "Child", .Tip = "A child switch.", .Needs = "Master" },
	{ .Section = "Video", .Key = "StrengthLevel", .Number = &Fields::Strength, .Default = 100, .Min = 1, .Max = 400,
	  .Page = "test", .Group = "Group B", .Label = "Strength", .Tip = "How strong.", .Needs = "ChildSwitch", .Unit = "%" },
	{ .Section = "Video", .Key = "Sharpness", .Number = &Fields::Sharpness, .Default = 0, .Min = 0, .Max = 4,
	  .Page = "test", .Group = "Group B", .Label = "Sharpness", .Tip = "How sharp.", .Unit = "x", .ZeroText = "Auto" },
	{ .Section = "Video", .Key = "Unbounded", .Number = &Fields::Unbounded, .Default = 5 },
	{ .Section = "Video", .Key = "Mode", .Number = &Fields::Mode, .Default = 0, .Names = Mode_Names,
	  .Page = "test", .Group = "Group A", .Label = "Mode", .Tip = "A mode shown as a switch.", .Needs = "Master", .SwitchOn = 1 },
};


void Read(INIClass & ini, std::string const & text)
{
	BufferStraw straw(text.data(), (int)text.size());
	ini.Load(straw, false);
}


std::string Save_Bytes(INIClass const & ini)
{
	std::string bytes(1 << 20, '\0');
	BufferPipe pipe(bytes.data(), (int)bytes.size());
	int const total = ini.Save(pipe);
	bytes.resize(total > 0 ? total : 0);
	return(bytes);
}


bool Value_Is(INIClass const & ini, char const * section, char const * entry, char const * expected)
{
	char buffer[INIClass::MAX_LINE_LENGTH];
	ini.Get_String(section, entry, "<missing>", buffer, sizeof(buffer));
	return(std::strcmp(buffer, expected) == 0);
}


void Test_Load_Save(void)
{
	INIClass ini;
	Read(ini, "[Video]\r\nMaster=no\r\nStrengthLevel=9999\r\nUnbounded=-7\r\nMode=Rain\r\nUnknownKey=keep me\r\n\r\n[Other]\r\nStranger=1\r\n");

	Fields fields;
	Settings_Reset(std::span<Setting<Fields> const>(Table), fields);
	Check(fields.Master && !fields.Child && fields.Strength == 100 && fields.Unbounded == 5 && fields.Mode == 0, "Reset gives every member its declared default");

	Settings_Load(std::span<Setting<Fields> const>(Table), ini, fields);
	Check(!fields.Master && !fields.Child, "a boolean key reads, and a missing one keeps its default");
	Check(fields.Strength == 400, "a value above the range is held to its maximum");
	Check(fields.Unbounded == -7, "a number without a range is read as written");
	Check(fields.Mode == 2, "a named value reads as its number");

	INIClass low;
	Read(low, "[Video]\r\nStrengthLevel=-3\r\nMode=Nonsense\r\n");
	Fields low_fields;
	Settings_Reset(std::span<Setting<Fields> const>(Table), low_fields);
	Settings_Load(std::span<Setting<Fields> const>(Table), low, low_fields);
	Check(low_fields.Strength == 1, "a value below the range is held to its minimum");
	Check(low_fields.Mode == 0, "an unknown name reads as the declared default");

	Settings_Save(std::span<Setting<Fields> const>(Table), ini, fields);
	Check(Value_Is(ini, "Video", "Master", "no") && Value_Is(ini, "Video", "StrengthLevel", "400") && Value_Is(ini, "Video", "Mode", "Rain"), "saving writes the values in the file's forms");
	Check(Value_Is(ini, "Video", "UnknownKey", "keep me"), "a key the table does not declare survives a save");
	Check(Value_Is(ini, "Other", "Stranger", "1"), "a section the table does not declare survives a save");
	Check(Value_Is(ini, "Video", "ChildSwitch", "no") && Value_Is(ini, "Video", "Unbounded", "-7"), "a key missing from the file is added by a save");
	std::string const saved = Save_Bytes(ini);
	Check(saved.find("UnknownKey") < saved.find("Master=") && saved.find("Master=") < saved.find("ChildSwitch") && saved.find("ChildSwitch") < saved.find("StrengthLevel") && saved.find("Unbounded") < saved.find("Mode="), "a save writes declared keys in table order after keys the table does not declare");
}


UISettingRow * Row_Of(std::vector<UISettingRow> & rows, char const * key)
{
	for (UISettingRow & row : rows) {
		if (row.Key == key) {
			return(&row);
		}
	}
	return(nullptr);
}


std::vector<UISettingRow> Test_Rows(Fields const & fields)
{
	return(UI_Setting_Rows(std::span<Setting<Fields> const>(Table), "test", [&](Setting<Fields> const & setting) { return(Setting_Value(setting, fields)); }));
}


void Test_Rows_And_Greying(void)
{
	Fields fields;
	fields.Master = false;
	fields.Mode = 2;
	std::vector<UISettingRow> rows = Test_Rows(fields);

	// Two groups appear in the order the table first names them, with the Mode row in Group A though the table lists it last.
	Check(rows.size() == 7 && rows[0].Kind == UISettingRowKind::HEADING && rows[0].Label == "Group A" && rows[4].Kind == UISettingRowKind::HEADING && rows[4].Label == "Group B", "each group gets a heading in first-appearance order");
	Check(rows[3].Key == "Mode", "a row follows its own group, not its place in the table");
	Check(Row_Of(rows, "Unbounded") == nullptr, "a declaration without a label gets no row");
	Check(Row_Of(rows, "Master")->Tip == "The master switch. Applies at the next start.", "a setting that needs a restart says so in its tip");

	bool modern = true;
	UISettingConditionCheck met = [&](SettingCondition condition) { return(condition != SettingCondition::MODERN_INTERFACE || modern); };
	UI_Setting_Rows_Refresh(rows, met);
	Check(!Row_Of(rows, "Master")->Disabled, "a row whose condition holds is enabled");
	Check(Row_Of(rows, "ChildSwitch")->Disabled && Row_Of(rows, "StrengthLevel")->Disabled, "a row greys while the setting it needs is off, through the chain");
	Check(Row_Of(rows, "ChildSwitch")->Classes == "check setting disabled" && Row_Of(rows, "StrengthLevel")->Classes == "setting slider-row disabled", "a greyed row carries the disabled class");
	Check(Row_Of(rows, "ChildSwitch")->ElementId == "child-switch" && Row_Of(rows, "ChildSwitch")->RowId == "switch-child-switch" && Row_Of(rows, "ChildSwitch")->Intent == "childswitch", "ids and intent names follow the key");

	UI_Setting_Row_Execute(rows, "childswitch", 1);
	Check(Row_Of(rows, "ChildSwitch")->Value == 0, "a greyed row ignores its intent");
	Check(UI_Setting_Row_Execute(rows, "childswitch", 1), "an intent naming a greyed row still counts as handled");
	Check(!UI_Setting_Row_Execute(rows, "nosuchrow", 1), "an intent no row names is left for the page");

	UI_Setting_Row_Execute(rows, "master", 1);
	UI_Setting_Rows_Refresh(rows, met);
	Check(Row_Of(rows, "Master")->Value == 1 && !Row_Of(rows, "ChildSwitch")->Disabled, "ticking the master enables its child");
	Check(Row_Of(rows, "Mode")->Value == 2, "a named switch that opened on a non-off value keeps it while its parent changes");

	UI_Setting_Row_Execute(rows, "mode", 0);
	Check(Row_Of(rows, "Mode")->Value == 0, "clearing a named switch stores its off value");
	UI_Setting_Row_Execute(rows, "mode", 1);
	Check(Row_Of(rows, "Mode")->Value == 2, "ticking a named switch again restores the value the page opened on");

	UI_Setting_Row_Execute(rows, "childswitch", 1);
	UI_Setting_Rows_Refresh(rows, met);
	Check(!Row_Of(rows, "StrengthLevel")->Disabled, "a chain enables once every link is on");
	UI_Setting_Row_Execute(rows, "strengthlevel", 9999);
	Check(Row_Of(rows, "StrengthLevel")->Value == 400, "a slider intent is held to the declared range");
	UI_Setting_Rows_Refresh(rows, met);
	Check(Row_Of(rows, "StrengthLevel")->ValueText == "400%" && Row_Of(rows, "Sharpness")->ValueText == "Auto", "a slider shows its number and unit, or its zero text at 0");
	UI_Setting_Row_Execute(rows, "sharpness", 2);
	UI_Setting_Rows_Refresh(rows, met);
	Check(Row_Of(rows, "Sharpness")->ValueText == "2x", "a slider with a zero text shows other values as numbers");
	UI_Setting_Row_Execute(rows, "sharpness", 0);

	modern = false;
	UI_Setting_Rows_Refresh(rows, met);
	Check(Row_Of(rows, "Master")->Disabled && Row_Of(rows, "ChildSwitch")->Disabled && Row_Of(rows, "StrengthLevel")->Disabled, "a false condition greys the row and everything that needs it");
	Check(Row_Of(rows, "Master")->Value == 1, "greying changes no value");

	Check(UI_Setting_Rows_Changed(rows), "a changed value is seen");
	UI_Setting_Rows_Reset(rows);
	Check(Row_Of(rows, "Master")->Value == 1 && Row_Of(rows, "StrengthLevel")->Value == 100 && Row_Of(rows, "Mode")->Value == 0, "Reset gives every row its declared default");

	modern = true;
	UI_Setting_Rows_Refresh(rows, met);
	Fields applied;
	Settings_Reset(std::span<Setting<Fields> const>(Table), applied);
	UI_Setting_Row_Execute(rows, "childswitch", 1);
	UI_Setting_Rows_Refresh(rows, met);
	UI_Setting_Row_Execute(rows, "strengthlevel", 50);
	UI_Setting_Rows_Refresh(rows, met);
	UI_Setting_Rows_Apply(std::span<Setting<Fields> const>(Table), rows, applied);
	Check(applied.Strength == 50 && applied.Master, "applying rows writes their values to the members");

	Check(UI_Setting_Element_Id("GpuVoxelSmooth") == "gpu-voxel-smooth" && UI_Setting_Element_Id("Fullscreen") == "fullscreen", "element ids are the key in lower case with dashes");
}


constexpr NameEntry Route_Names[] = {
	{ "Plain", 1 },
	{ "Fancy", 2 },
};


constexpr Setting<Fields> Default_On_Table[] = {
	{ .Section = "Video", .Key = "Mode", .Number = &Fields::Mode, .Default = 2, .Names = Route_Names,
	  .Page = "test", .Group = "Group A", .Label = "Mode", .Tip = "A mode that starts switched on.", .SwitchOn = 2, .Off = 1 },
};


void Test_Switch_Default_On(void)
{
	std::vector<UISettingRow> rows = UI_Setting_Rows(std::span<Setting<Fields> const>(Default_On_Table), "test", [](Setting<Fields> const & setting) { return(setting.Default); });
	UI_Setting_Rows_Refresh(rows, [](SettingCondition) { return(true); });
	Check(Row_Of(rows, "Mode") != nullptr && Row_Of(rows, "Mode")->Kind == UISettingRowKind::SWITCH && Row_Of(rows, "Mode")->Value == 2, "a switch whose setting defaults to its switched-on value starts ticked");
	UI_Setting_Row_Execute(rows, "mode", 0);
	Check(Row_Of(rows, "Mode")->Value == 1, "clearing that switch stores its off value");
	UI_Setting_Row_Execute(rows, "mode", 1);
	Check(Row_Of(rows, "Mode")->Value == 2, "ticking it again stores its switched-on value");
}


static NameEntry const Layer_Depth_Names[] = { { "Shared", GPU_VOXEL_LAYER_DEPTH_SHARED }, { "Ordered", GPU_VOXEL_LAYER_DEPTH_ORDERED } };
static NameEntry const Cloak_Names[] = { { "Translucent", GPU_VOXEL_CLOAK_TRANSLUCENT }, { "Ripple", GPU_VOXEL_CLOAK_RIPPLE } };
static NameEntry const Overlay_Names[] = { { "Off", PERF_OVERLAY_OFF }, { "Basic", PERF_OVERLAY_BASIC }, { "Detailed", PERF_OVERLAY_DETAILED } };
static NameEntry const ZoomOut_Names[] = { { "Original", ZOOM_OUT_VIEW_ORIGINAL }, { "4K", ZOOM_OUT_VIEW_4K } };


// The GPU options as OptionsClass reads and writes them today; kept here to pin the table against that code.
struct Old_Gpu
{
	bool GpuRender = true;
	bool GpuVoxels = false;
	bool GpuVoxelSmooth = false;
	bool GpuVoxelSubpixel = false;
	int GpuVoxelScale = 0;
	int GpuVoxelLayerDepth = GPU_VOXEL_LAYER_DEPTH_SHARED;
	int GpuVoxelCloak = GPU_VOXEL_CLOAK_TRANSLUCENT;
	bool SteadyVoxelPlacement = true;
	bool ShadowStacking = true;
	bool StockArtFixes = true;
	bool RenderThread = false;
	bool FastestGpu = true;
	bool ParallelPaths = false;
	int PathThreads = 0;
	int PerfOverlay = PERF_OVERLAY_OFF;
	int ZoomOutView = ZOOM_OUT_VIEW_ORIGINAL;
	bool ClassicGameSpeeds = false;
};


void Old_Load(INIClass const & ini, Old_Gpu & old)
{
	old.GpuRender = ini.Get_Bool("Video", "GpuRender", old.GpuRender);
	old.GpuVoxels = ini.Get_Bool("Video", "GpuVoxels", old.GpuVoxels);
	old.GpuVoxelSmooth = ini.Get_Bool("Video", "GpuVoxelSmooth", old.GpuVoxelSmooth);
	old.GpuVoxelSubpixel = ini.Get_Bool("Video", "GpuVoxelSubpixel", old.GpuVoxelSubpixel);
	old.GpuVoxelScale = std::clamp(ini.Get_Int("Video", "GpuVoxelScale", old.GpuVoxelScale), 0, 4);
	char name[16];
	ini.Get_String("Video", "GpuVoxelLayerDepth", "Shared", name, sizeof(name));
	old.GpuVoxelLayerDepth = Enum_From_Name(Layer_Depth_Names, name, GPU_VOXEL_LAYER_DEPTH_SHARED);
	ini.Get_String("Video", "GpuVoxelCloak", "Translucent", name, sizeof(name));
	old.GpuVoxelCloak = Enum_From_Name(Cloak_Names, name, GPU_VOXEL_CLOAK_TRANSLUCENT);
	old.SteadyVoxelPlacement = ini.Get_Bool("Video", "SteadyVoxelPlacement", old.SteadyVoxelPlacement);
	old.ShadowStacking = ini.Get_Bool("Video", "ShadowStacking", old.ShadowStacking);
	old.StockArtFixes = ini.Get_Bool("Video", "StockArtFixes", old.StockArtFixes);
	old.RenderThread = ini.Get_Bool("Video", "RenderThread", old.RenderThread);
	old.FastestGpu = ini.Get_Bool("Video", "FastestGpu", old.FastestGpu);
	old.ParallelPaths = ini.Get_Bool("Options", "ParallelPaths", old.ParallelPaths);
	old.PathThreads = std::clamp(ini.Get_Int("Options", "PathThreads", old.PathThreads), 0, 32);
	ini.Get_String("Video", "PerfOverlay", "Off", name, sizeof(name));
	old.PerfOverlay = Enum_From_Name(Overlay_Names, name, PERF_OVERLAY_OFF);
	ini.Get_String("Video", "ZoomOutView", "Original", name, sizeof(name));
	old.ZoomOutView = Enum_From_Name(ZoomOut_Names, name, ZOOM_OUT_VIEW_ORIGINAL);
	old.ClassicGameSpeeds = ini.Get_Bool("Options", "ClassicGameSpeeds", old.ClassicGameSpeeds);
}


void Old_Save(INIClass & ini, Old_Gpu const & old)
{
	ini.Put_Bool("Video", "GpuRender", old.GpuRender);
	ini.Put_Bool("Video", "GpuVoxels", old.GpuVoxels);
	ini.Put_Bool("Video", "GpuVoxelSmooth", old.GpuVoxelSmooth);
	ini.Put_Bool("Video", "GpuVoxelSubpixel", old.GpuVoxelSubpixel);
	ini.Put_Int("Video", "GpuVoxelScale", old.GpuVoxelScale);
	ini.Put_String("Video", "GpuVoxelLayerDepth", Enum_Name(Layer_Depth_Names, old.GpuVoxelLayerDepth, "Shared"));
	ini.Put_String("Video", "GpuVoxelCloak", Enum_Name(Cloak_Names, old.GpuVoxelCloak, "Translucent"));
	ini.Put_Bool("Video", "SteadyVoxelPlacement", old.SteadyVoxelPlacement);
	ini.Put_Bool("Video", "ShadowStacking", old.ShadowStacking);
	ini.Put_Bool("Video", "StockArtFixes", old.StockArtFixes);
	ini.Put_Bool("Video", "RenderThread", old.RenderThread);
	ini.Put_Bool("Video", "FastestGpu", old.FastestGpu);
	ini.Put_Bool("Options", "ParallelPaths", old.ParallelPaths);
	ini.Put_Int("Options", "PathThreads", old.PathThreads);
	ini.Put_String("Video", "PerfOverlay", Enum_Name(Overlay_Names, old.PerfOverlay, "Off"));
	ini.Put_String("Video", "ZoomOutView", Enum_Name(ZoomOut_Names, old.ZoomOutView, "Original"));
	ini.Put_Bool("Options", "ClassicGameSpeeds", old.ClassicGameSpeeds);
}


// The real table's members in a blank instance: OptionsClass cannot be constructed here, and only declared bool and int members are touched.
struct Blank_Options
{
	alignas(OptionsClass) unsigned char Storage[sizeof(OptionsClass)] = {};
	OptionsClass & Get(void) { return(*reinterpret_cast<OptionsClass *>(Storage)); }
};


// Runs the file through the old load and save and through the declared table, and reports any difference in the bytes written.
bool Round_Trip_Matches(std::string const & text, char const * label)
{
	INIClass old_ini;
	Read(old_ini, text);
	Old_Gpu old;
	Old_Load(old_ini, old);
	Old_Save(old_ini, old);

	INIClass new_ini;
	Read(new_ini, text);
	Blank_Options blank;
	Settings_Reset(Option_Settings(), blank.Get());
	Settings_Load(Option_Settings(), new_ini, blank.Get());
	Settings_Save(Option_Settings(), new_ini, blank.Get());

	std::string const old_bytes = Save_Bytes(old_ini);
	std::string const new_bytes = Save_Bytes(new_ini);
	if (old_bytes == new_bytes) {
		return(true);
	}
	std::printf("%s: the round trip differs from the old code\n--- old\n%s\n--- new\n%s\n", label, old_bytes.c_str(), new_bytes.c_str());
	return(false);
}


void Test_Real_Table(void)
{
	std::span<Setting<OptionsClass> const> settings = Option_Settings();

	std::vector<std::string> keys;
	for (Setting<OptionsClass> const & setting : settings) {
		keys.push_back(setting.Key);
		Check((setting.Flag != nullptr) != (setting.Number != nullptr), "a declaration names exactly one member");
		if (setting.Needs != nullptr) {
			bool found = false;
			for (Setting<OptionsClass> const & other : settings) {
				found = found || (std::strcmp(other.Key, setting.Needs) == 0 && other.Page != nullptr && std::strcmp(other.Page, setting.Page) == 0);
			}
			Check(found, "a declaration needs a setting that has a row on the same page");
		}
		if (setting.Label != nullptr && !setting.Names.empty()) {
			Check(setting.SwitchOn != 0, "a named setting shown as a row is a switch");
		}
	}

	std::vector<std::string> const old_order = {
		"GpuRender", "GpuVoxels", "GpuVoxelSmooth", "GpuVoxelSubpixel", "GpuVoxelScale", "GpuVoxelLayerDepth", "GpuVoxelCloak", "SteadyVoxelPlacement", "ShadowStacking", "StockArtFixes",
		"RenderThread", "FastestGpu", "ParallelPaths", "PathThreads", "PerfOverlay", "ZoomOutView", "ClassicGameSpeeds",
	};
	Check(keys == old_order, "the table lists the [Video] GPU keys in the order the settings file has always had them, then the path search keys and the game speed switch");

	std::vector<UISettingRow> rows = UI_Setting_Rows(settings, "graphics", [](Setting<OptionsClass> const & setting) { return(setting.Default); }, Option_Setting_Presets());
	std::vector<std::string> labels;
	for (UISettingRow const & row : rows) {
		labels.push_back(row.Label);
	}
	std::vector<std::string> const page_order = {
		"Graphics quality", "Preset", "Advanced",
		"Graphics card", "Draw with the graphics card",
		"Vehicles and aircraft", "Draw vehicles and aircraft in 3D", "Smooth turning", "Smooth movement when zoomed in", "Vehicle sharpness",
		"Vehicle details", "Steady vehicle placement", "Darker overlapping shadows",
		"Stock art", "Fix stock art",
		"Diagnostics", "Performance overlay",
		"Camera", "Zoom out to a 4K view",
	};
	Check(labels == page_order, "the Graphics page shows the quality choice, then the Advanced fold with the GPU drawing row and the vehicle rows after it, then the other groups");
	std::vector<std::string> folded;
	for (UISettingRow const & row : rows) {
		if (!row.Fold.empty()) {
			folded.push_back(row.Label);
		}
	}
	Check(folded.size() == 10 && folded.front() == "Graphics card" && folded.back() == "Darker overlapping shadows", "every row and heading from the GPU drawing group to the vehicle details sits under the Advanced fold, and no other does");

	char const * const texts[] = {
		"[Video]\r\n",
		"[Video]\r\nGpuRender=no\r\nSoftShadows=yes\r\nSoftShadowRadius=99\r\nBloom=1\r\nBloomStrength=0\r\nWeather=Rain\r\nWeatherStrength=999\r\nGpuVoxelScale=9\r\nGpuVoxelPath=Direct\r\nGpuVoxelLayerDepth=Ordered\r\nGpuVoxelCloak=Ripple\r\nCustomKey=abc\r\n\r\n[Options]\r\nGameSpeed=1\r\n",
		"[Video]\r\nGpuVoxelPath=Image\r\n",
		"[Video]\r\nPerfOverlay=Detailed\r\n",
		"[Video]\r\nPerfOverlay=Loud\r\n",
		"[Video]\r\nZoomOutView=4K\r\n",
		"[Video]\r\nZoomOutView=Unlimited\r\n",
		"[Video]\r\nWeather=Bogus\r\nGpuVoxelPath=Sideways\r\nGpuVoxelCloak=Invisible\r\nGpuVoxels=true\r\nCloudShadowStrength=not a number\r\nWaterRipples=0\r\nShockwaves=TRUE\r\n",
		"[Video]\r\nGpuRender=yes\r\nSoftShadows=no\r\nSoftShadowRadius=3\r\nShockwaves=no\r\nBloom=no\r\nBloomStrength=100\r\nWaterRipples=no\r\nWaterRippleStrength=100\r\nCloudShadows=no\r\nCloudShadowStrength=100\r\nGpuVoxels=no\r\nGpuVoxelSmooth=no\r\nGpuVoxelScale=1\r\nWeather=None\r\nWeatherStrength=100\r\n",
	};
	for (char const * text : texts) {
		Check(Round_Trip_Matches(text, "fixture"), "the declared table reads and writes a settings file exactly as the old code did, and keeps the keys of removed settings as they were");
	}
}


// The Modern in-game interface switch the Graphics page draws by hand, as the page gives it to its presets.
int Modern_Interface_Switch = 1;


// The rows of the Graphics page for the settings a file text holds, as the page opens on them with the interface switch
// at modern.
std::vector<UISettingRow> Graphics_Rows_Of(std::string const & text, int modern = 1)
{
	INIClass ini;
	Read(ini, text);
	Blank_Options blank;
	Settings_Reset(Option_Settings(), blank.Get());
	Settings_Load(Option_Settings(), ini, blank.Get());
	OptionsClass & options = blank.Get();
	std::vector<UISettingRow> rows = UI_Setting_Rows(Option_Settings(), "graphics", [&](Setting<OptionsClass> const & setting) { return(Setting_Value(setting, options)); }, Option_Setting_Presets());
	Modern_Interface_Switch = modern;
	UISettingOutsideRows outside;
	outside.Get = [](std::string const & key, int & value) {
		if (key != MODERN_INTERFACE_ROW) {
			return(false);
		}
		value = Modern_Interface_Switch;
		return(true);
	};
	outside.Set = [](std::string const & key, int value) {
		if (key == MODERN_INTERFACE_ROW) {
			Modern_Interface_Switch = value;
		}
	};
	UI_Setting_Rows_Set_Outside(rows, outside);
	return(rows);
}


UISettingRow * Quality(std::vector<UISettingRow> & rows)
{
	return(Row_Of(rows, "GraphicsQuality"));
}


std::string Quality_Text(std::vector<UISettingRow> & rows)
{
	UISettingRow * row = Quality(rows);
	return((row != nullptr && row->Value >= 0 && row->Value < (int)row->Items.size()) ? row->Items[row->Value] : "?");
}


void Test_Presets(void)
{
	std::span<SettingPresetChoice const> const choices = Option_Setting_Presets();
	Check(choices.size() == 1 && choices[0].Presets.size() == 3, "the Graphics page declares one choice over three presets");
	std::string const third = choices[0].Presets[2].Label;

	// A preset names only rows of its page, and Enhanced holds the default of each.
	std::vector<UISettingRow> defaults = Graphics_Rows_Of("[Video]\r\n");
	for (SettingPreset const & preset : choices[0].Presets) {
		for (SettingPresetValue const & value : preset.Values) {
			Check(Row_Of(defaults, value.Key) != nullptr || std::strcmp(value.Key, MODERN_INTERFACE_ROW) == 0, "a preset names a row of its page");
		}
	}
	for (SettingPresetValue const & value : choices[0].Presets[1].Values) {
		UISettingRow const * row = Row_Of(defaults, value.Key);
		Check(row == nullptr || (row->Value == row->Default && value.Value == row->Default), "Enhanced holds the default of each declared switch it covers");
	}

	Check(Quality_Text(defaults) == "Enhanced", "a settings file with no graphics keys, in the modern interface, reads as Enhanced");
	Check(Quality(defaults)->Items.size() == 4 && Quality(defaults)->Items.back() == "Custom", "the choice lists the presets, then Custom");
	Check(!UI_Setting_Rows_Changed(defaults), "an unchanged page reports no change");

	UI_Setting_Rows_Refresh(defaults, [](SettingCondition) { return(true); });
	UI_Setting_Row_Execute(defaults, "graphicsquality", 0);
	Check(Quality_Text(defaults) == "Classic" && Modern_Interface_Switch == 0 && Row_Of(defaults, "GpuRender")->Value == 0 && Row_Of(defaults, "GpuVoxels")->Value == 0,
		"Classic turns off the modern interface and drawing with the graphics card");
	Check(UI_Setting_Rows_Changed(defaults), "a preset counts as a change");

	UI_Setting_Row_Execute(defaults, "graphicsquality", 2);
	Check(Quality_Text(defaults) == third && Modern_Interface_Switch == 1 && Row_Of(defaults, "GpuRender")->Value == 1 && Row_Of(defaults, "GpuVoxels")->Value == 1
		&& Row_Of(defaults, "GpuVoxelSmooth")->Value == 1 && Row_Of(defaults, "GpuVoxelSubpixel")->Value == 1 && Row_Of(defaults, "GpuVoxelScale")->Value == 4,
		"the third preset draws vehicles in 3D with smooth turning and movement at sharpness 4");

	UI_Setting_Row_Execute(defaults, "graphicsquality", 1);
	Check(Quality_Text(defaults) == "Enhanced" && Row_Of(defaults, "GpuVoxels")->Value == 0 && Row_Of(defaults, "GpuVoxelScale")->Value == 4,
		"Enhanced turns the vehicle switches off again and leaves the sharpness, which it does not name");

	UI_Setting_Row_Execute(defaults, "graphicsquality", 2);
	UI_Setting_Row_Execute(defaults, "gpuvoxelsmooth", 0);
	Check(Quality_Text(defaults) == "Custom", "changing one covered switch by hand reads as Custom");
	UI_Setting_Rows_Refresh(defaults, [](SettingCondition) { return(true); });
	UI_Setting_Row_Execute(defaults, "graphicsquality", 3);
	Check(Quality_Text(defaults) == "Custom" && Row_Of(defaults, "GpuVoxelSmooth")->Value == 0 && Row_Of(defaults, "GpuVoxels")->Value == 1, "choosing Custom changes nothing");
	UI_Setting_Row_Execute(defaults, "gpuvoxelsmooth", 1);
	Check(Quality_Text(defaults) == third, "putting the switch back reads as the third preset again");
	Modern_Interface_Switch = 0;
	UI_Setting_Rows_Sync_Presets(defaults);
	Check(Quality_Text(defaults) == "Custom", "the interface switch counts toward the reading");
	Modern_Interface_Switch = 1;

	UI_Setting_Row_Execute(defaults, "steadyvoxelplacement", 0);
	Check(Quality_Text(defaults) == third, "a switch no preset covers does not change the reading");
	UI_Setting_Rows_Reset(defaults);
	Check(Quality_Text(defaults) == "Enhanced", "Reset to defaults reads as Enhanced");

	// Files an older build wrote hold only the keys it knew, and the preset reads from the values alone. Keys of removed
	// settings are ignored.
	std::vector<UISettingRow> old_full = Graphics_Rows_Of("[Video]\r\nGpuRender=yes\r\nSoftShadows=yes\r\nShockwaves=yes\r\nBloom=yes\r\nWaterRipples=yes\r\nCloudShadows=yes\r\nWeather=Map\r\nGpuVoxels=yes\r\nGpuVoxelSmooth=yes\r\nGpuVoxelSubpixel=yes\r\nGpuVoxelScale=4\r\n");
	Check(Quality_Text(old_full) == third, "an older settings file with the old Full switches and sharpness 4 reads as the third preset");
	std::vector<UISettingRow> old_effects = Graphics_Rows_Of("[Video]\r\nSoftShadows=yes\r\nBloom=yes\r\nWeather=Rain\r\n");
	Check(Quality_Text(old_effects) == "Enhanced", "an older settings file with only removed effects reads as Enhanced");
	std::vector<UISettingRow> classic = Graphics_Rows_Of("[Video]\r\nGpuRender=no\r\n", 0);
	Check(Quality_Text(classic) == "Classic", "software drawing in the classic interface reads as Classic");
	std::vector<UISettingRow> software = Graphics_Rows_Of("[Video]\r\nGpuRender=no\r\n");
	Check(Quality_Text(software) == "Custom", "software drawing in the modern interface reads as Custom");
	std::vector<UISettingRow> half = Graphics_Rows_Of("[Video]\r\nGpuVoxels=yes\r\n");
	Check(Quality_Text(half) == "Custom", "a settings file that holds some of a preset's switches reads as Custom");

	// The fold opens and closes, hides its rows while closed, and is not a setting.
	std::vector<UISettingRow> rows = Graphics_Rows_Of("[Video]\r\n");
	auto met = [](SettingCondition) { return(true); };
	UI_Setting_Rows_Refresh(rows, met);
	UISettingRow * fold = Row_Of(rows, "Advanced");
	Check(fold != nullptr && fold->Kind == UISettingRowKind::FOLD && fold->Value == 0 && fold->ValueText == "[+] Show Advanced", "the Advanced fold starts closed and says what pressing it does");
	Check(Row_Of(rows, "GpuRender")->Hidden && Row_Of(rows, "GpuRender")->Classes.find("folded") != std::string::npos && !Row_Of(rows, "ZoomOutView")->Hidden && !Quality(rows)->Hidden, "a closed fold hides its rows, and the others show");
	UI_Setting_Row_Execute(rows, "advanced", 1);
	UI_Setting_Rows_Refresh(rows, met);
	Check(Row_Of(rows, "Advanced")->ValueText == "[-] Hide Advanced" && !Row_Of(rows, "GpuRender")->Hidden, "opening the fold shows its rows");
	Check(!UI_Setting_Rows_Changed(rows), "opening a fold is not a change");
	UI_Setting_Rows_Reset(rows);
	Check(Row_Of(rows, "Advanced")->Value == 1, "Reset to defaults leaves the fold as it is");

	UI_Setting_Rows_Refresh(rows, [](SettingCondition) { return(false); });
	Check(!Quality(rows)->Disabled, "the choice stays open without the modern interface, so Enhanced can turn it back on");
}


void Round_Trip_File(char const * path)
{
	std::FILE * file = std::fopen(path, "rb");
	if (file == nullptr) {
		std::printf("FAIL: cannot read %s\n", path);
		Failures++;
		return;
	}
	std::string text;
	char buffer[4096];
	size_t got;
	while ((got = std::fread(buffer, 1, sizeof(buffer), file)) > 0) {
		text.append(buffer, got);
	}
	std::fclose(file);

	Check(Round_Trip_Matches(text, path), "the given settings file round-trips as the old code did");

	INIClass original;
	Read(original, text);
	INIClass saved;
	Read(saved, text);
	Blank_Options blank;
	Settings_Reset(Option_Settings(), blank.Get());
	Settings_Load(Option_Settings(), saved, blank.Get());
	Settings_Save(Option_Settings(), saved, blank.Get());
	bool const unchanged = (Save_Bytes(original) == Save_Bytes(saved));
	std::printf("%s: %s after loading and saving the declared keys\n", path, unchanged ? "byte-identical" : "keys added or rewritten");
}


void Test_Load_Before_Renderer(void)
{
	static constexpr Setting<Fields> Early[] = {
		{ .Section = "Video", .Key = "Master", .Flag = &Fields::Master, .Default = true, .BeforeRenderer = true },
		{ .Section = "Video", .Key = "Unbounded", .Number = &Fields::Unbounded, .Default = 5 },
	};
	INIClass ini;
	Read(ini, "[Video]\r\nMaster=no\r\nUnbounded=-7\r\n");
	Fields fields;
	Settings_Reset(std::span<Setting<Fields> const>(Early), fields);
	Settings_Load_Before_Renderer(std::span<Setting<Fields> const>(Early), ini, fields);
	Check(!fields.Master, "a setting marked BeforeRenderer is read before the renderer starts");
	Check(fields.Unbounded == 5, "an unmarked setting waits for Settings_Load");
}

}


int main(int argc, char * argv[])
{
	Test_Load_Before_Renderer();
	Test_Load_Save();
	Test_Rows_And_Greying();
	Test_Switch_Default_On();
	Test_Real_Table();
	Test_Presets();
	if (argc > 1) {
		Round_Trip_File(argv[1]);
	}
	if (Failures > 0) {
		std::printf("%d of %d checks failed\n", Failures, Checked);
		return(1);
	}
	std::printf("settingregistry: %d checks passed\n", Checked);
	return(0);
}
