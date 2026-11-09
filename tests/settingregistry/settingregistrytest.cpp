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
	Check(rows.size() == 6 && rows[0].Kind == UISettingRowKind::HEADING && rows[0].Label == "Group A" && rows[4].Kind == UISettingRowKind::HEADING && rows[4].Label == "Group B", "each group gets a heading in first-appearance order");
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

	Check(UI_Setting_Element_Id("GpuVoxelSmooth") == "gpu-voxel-smooth" && UI_Setting_Element_Id("Bloom") == "bloom", "element ids are the key in lower case with dashes");
}


// The GPU options as OptionsClass reads and writes them today; kept here to pin the table against that code.
struct Old_Gpu
{
	bool GpuRender = true;
	bool SoftShadows = false;
	int SoftShadowRadius = 3;
	bool Shockwaves = false;
	bool Bloom = false;
	int BloomStrength = 100;
	bool WaterRipples = false;
	int WaterRippleStrength = 100;
	bool CloudShadows = false;
	int CloudShadowStrength = 100;
	bool GpuVoxels = false;
	bool GpuVoxelSmooth = false;
	int GpuVoxelScale = 1;
	int Weather = WEATHER_NONE;
	int WeatherStrength = 100;
	bool RenderThread = false;
};


void Old_Load(INIClass const & ini, Old_Gpu & old)
{
	old.GpuRender = ini.Get_Bool("Video", "GpuRender", old.GpuRender);
	old.SoftShadows = ini.Get_Bool("Video", "SoftShadows", old.SoftShadows);
	old.SoftShadowRadius = std::clamp(ini.Get_Int("Video", "SoftShadowRadius", old.SoftShadowRadius), 1, 16);
	old.Shockwaves = ini.Get_Bool("Video", "Shockwaves", old.Shockwaves);
	old.Bloom = ini.Get_Bool("Video", "Bloom", old.Bloom);
	old.BloomStrength = std::clamp(ini.Get_Int("Video", "BloomStrength", old.BloomStrength), 1, 400);
	old.WaterRipples = ini.Get_Bool("Video", "WaterRipples", old.WaterRipples);
	old.WaterRippleStrength = std::clamp(ini.Get_Int("Video", "WaterRippleStrength", old.WaterRippleStrength), 1, 400);
	old.CloudShadows = ini.Get_Bool("Video", "CloudShadows", old.CloudShadows);
	old.CloudShadowStrength = std::clamp(ini.Get_Int("Video", "CloudShadowStrength", old.CloudShadowStrength), 1, 400);
	old.GpuVoxels = ini.Get_Bool("Video", "GpuVoxels", old.GpuVoxels);
	old.GpuVoxelSmooth = ini.Get_Bool("Video", "GpuVoxelSmooth", old.GpuVoxelSmooth);
	old.GpuVoxelScale = std::clamp(ini.Get_Int("Video", "GpuVoxelScale", old.GpuVoxelScale), 1, 4);
	char weathername[16];
	ini.Get_String("Video", "Weather", "None", weathername, sizeof(weathername));
	static NameEntry const Names[] = { { "None", WEATHER_NONE }, { "Map", WEATHER_MAP }, { "Rain", WEATHER_RAIN }, { "Snow", WEATHER_SNOW } };
	old.Weather = Enum_From_Name(Names, weathername, WEATHER_NONE);
	old.WeatherStrength = std::clamp(ini.Get_Int("Video", "WeatherStrength", old.WeatherStrength), 1, 400);
	old.RenderThread = ini.Get_Bool("Video", "RenderThread", old.RenderThread);
}


void Old_Save(INIClass & ini, Old_Gpu const & old)
{
	ini.Put_Bool("Video", "GpuRender", old.GpuRender);
	ini.Put_Bool("Video", "SoftShadows", old.SoftShadows);
	ini.Put_Int("Video", "SoftShadowRadius", old.SoftShadowRadius);
	ini.Put_Bool("Video", "Shockwaves", old.Shockwaves);
	ini.Put_Bool("Video", "Bloom", old.Bloom);
	ini.Put_Int("Video", "BloomStrength", old.BloomStrength);
	ini.Put_Bool("Video", "WaterRipples", old.WaterRipples);
	ini.Put_Int("Video", "WaterRippleStrength", old.WaterRippleStrength);
	ini.Put_Bool("Video", "CloudShadows", old.CloudShadows);
	ini.Put_Int("Video", "CloudShadowStrength", old.CloudShadowStrength);
	ini.Put_Bool("Video", "GpuVoxels", old.GpuVoxels);
	ini.Put_Bool("Video", "GpuVoxelSmooth", old.GpuVoxelSmooth);
	ini.Put_Int("Video", "GpuVoxelScale", old.GpuVoxelScale);
	static NameEntry const Names[] = { { "None", WEATHER_NONE }, { "Map", WEATHER_MAP }, { "Rain", WEATHER_RAIN }, { "Snow", WEATHER_SNOW } };
	ini.Put_String("Video", "Weather", Enum_Name(Names, std::clamp(old.Weather, (int)WEATHER_NONE, (int)WEATHER_SNOW), "None"));
	ini.Put_Int("Video", "WeatherStrength", old.WeatherStrength);
	ini.Put_Bool("Video", "RenderThread", old.RenderThread);
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
		"GpuRender", "SoftShadows", "SoftShadowRadius", "Shockwaves", "Bloom", "BloomStrength", "WaterRipples", "WaterRippleStrength",
		"CloudShadows", "CloudShadowStrength", "GpuVoxels", "GpuVoxelSmooth", "GpuVoxelScale", "Weather", "WeatherStrength", "RenderThread",
	};
	Check(keys == old_order, "the table lists the [Video] GPU keys in the order the settings file has always had them");

	std::vector<UISettingRow> rows = UI_Setting_Rows(settings, "graphics", [](Setting<OptionsClass> const & setting) { return(setting.Default); });
	std::vector<std::string> labels;
	for (UISettingRow const & row : rows) {
		labels.push_back(row.Label);
	}
	std::vector<std::string> const page_order = {
		"GPU drawing", "Draw units on the GPU", "Soft shadows", "Shockwaves", "Glow", "Water ripples", "Cloud shadows", "Weather",
		"GPU voxels", "Draw voxels on the GPU", "Smooth voxel turning", "Voxel sharpness",
	};
	Check(labels == page_order, "the Graphics page keeps the GPU drawing rows in their old order, with the voxel rows after them");

	char const * const texts[] = {
		"[Video]\r\n",
		"[Video]\r\nGpuRender=no\r\nSoftShadows=yes\r\nSoftShadowRadius=99\r\nBloom=1\r\nBloomStrength=0\r\nWeather=Rain\r\nWeatherStrength=999\r\nGpuVoxelScale=9\r\nCustomKey=abc\r\n\r\n[Options]\r\nGameSpeed=1\r\n",
		"[Video]\r\nWeather=Bogus\r\nGpuVoxels=true\r\nCloudShadowStrength=not a number\r\nWaterRipples=0\r\nShockwaves=TRUE\r\n",
		"[Video]\r\nGpuRender=yes\r\nSoftShadows=no\r\nSoftShadowRadius=3\r\nShockwaves=no\r\nBloom=no\r\nBloomStrength=100\r\nWaterRipples=no\r\nWaterRippleStrength=100\r\nCloudShadows=no\r\nCloudShadowStrength=100\r\nGpuVoxels=no\r\nGpuVoxelSmooth=no\r\nGpuVoxelScale=1\r\nWeather=None\r\nWeatherStrength=100\r\n",
	};
	for (char const * text : texts) {
		Check(Round_Trip_Matches(text, "fixture"), "the declared table reads and writes a settings file exactly as the old code did");
	}
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
	Test_Real_Table();
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
