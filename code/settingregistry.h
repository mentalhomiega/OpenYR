/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// One declaration per client setting: the file key, the value it holds and how its settings row looks.
// docs/SETTINGS.md describes how to add one.

#pragma once

#include "always.h"

#include "ini.h"
#include "nametable.h"

#include <algorithm>
#include <span>
#include <type_traits>


// Greys a row for a reason the settings file does not hold. The page that shows the row answers it.
enum class SettingCondition
{
	NONE,
	MODERN_INTERFACE
};


// One client setting. A table of these is the only place the setting's key, default, limits and row are written.
template<typename Owner>
struct Setting
{
	char const * Section = nullptr;
	char const * Key = nullptr;

	// The member that holds the value: exactly one of these is set.
	bool Owner::* Flag = nullptr;
	int Owner::* Number = nullptr;

	int Default = 0;

	// A Number is limited to Min and Max when it is read, unless they are equal.
	int Min = 0;
	int Max = 0;

	// When set, a Number is written to the file as one of these names, and an unknown name reads as Default.
	std::span<NameEntry const> Names = {};

	// The settings page and group the row appears in, with its text. A null Label gives no row.
	char const * Page = nullptr;
	char const * Group = nullptr;
	char const * Label = nullptr;
	char const * Tip = nullptr;

	// The key of another setting on the page. The row is greyed while that setting is off or greyed.
	char const * Needs = nullptr;
	SettingCondition Condition = SettingCondition::NONE;

	// The tip then says that a change applies at the next start.
	bool NeedsRestart = false;

	// A Names setting shown as a switch: the value ticking it picks, unless the page opened on another non-off value.
	int SwitchOn = 0;

	// The value of that switch when it is cleared; the setting's default may be the switched-on value.
	int Off = 0;

	// The text after a slider's number, such as "%".
	char const * Unit = "";

	// The text a slider shows in place of the number 0, such as "Auto".
	char const * ZeroText = "";

	// Read by Settings_Load_Before_Renderer too, for a setting the renderer needs when it starts.
	bool BeforeRenderer = false;
};


// A settings page made only of declared rows: the rows whose Page is Id. docs/SETTINGS.md describes how to add one.
struct SettingPage
{
	char const * Id = nullptr;
	// The text of the page's tab and of its button in the options menu.
	char const * Label = nullptr;
};


// Enum names longer than this are not read back whole.
enum { SETTING_NAME_SIZE = 16 };


template<typename Owner>
int Setting_Value(Setting<Owner> const & setting, std::type_identity_t<Owner> const & owner)
{
	return((setting.Flag != nullptr) ? (owner.*setting.Flag ? 1 : 0) : owner.*setting.Number);
}


template<typename Owner>
void Setting_Set(Setting<Owner> const & setting, std::type_identity_t<Owner> & owner, int value)
{
	if (setting.Flag != nullptr) {
		owner.*setting.Flag = (value != 0);
	} else {
		owner.*setting.Number = value;
	}
}


// A key the file does not hold keeps the member's value, as Settings_Reset set it.
template<typename Owner>
void Setting_Load(Setting<Owner> const & setting, INIClass const & ini, std::type_identity_t<Owner> & owner)
{
	if (setting.Flag != nullptr) {
		owner.*setting.Flag = ini.Get_Bool(setting.Section, setting.Key, owner.*setting.Flag);
	} else if (!setting.Names.empty()) {
		char name[SETTING_NAME_SIZE];
		ini.Get_String(setting.Section, setting.Key, Enum_Name(setting.Names, setting.Default, ""), name, sizeof(name));
		owner.*setting.Number = Enum_From_Name(setting.Names, name, setting.Default);
	} else {
		int const value = ini.Get_Int(setting.Section, setting.Key, owner.*setting.Number);
		owner.*setting.Number = (setting.Min < setting.Max) ? std::clamp(value, setting.Min, setting.Max) : value;
	}
}


template<typename Owner>
void Setting_Save(Setting<Owner> const & setting, INIClass & ini, std::type_identity_t<Owner> const & owner)
{
	if (setting.Flag != nullptr) {
		ini.Put_Bool(setting.Section, setting.Key, owner.*setting.Flag);
	} else if (!setting.Names.empty()) {
		ini.Put_String(setting.Section, setting.Key, Enum_Name(setting.Names, owner.*setting.Number, Enum_Name(setting.Names, setting.Default, "")));
	} else {
		ini.Put_Int(setting.Section, setting.Key, owner.*setting.Number);
	}
}


// Sets every declared member to its default. A table is constant-initialized, so a global owner may call this from its constructor.
template<typename Owner>
void Settings_Reset(std::span<Setting<Owner> const> settings, std::type_identity_t<Owner> & owner)
{
	for (Setting<Owner> const & setting : settings) {
		Setting_Set(setting, owner, setting.Default);
	}
}


template<typename Owner>
void Settings_Load(std::span<Setting<Owner> const> settings, INIClass const & ini, std::type_identity_t<Owner> & owner)
{
	for (Setting<Owner> const & setting : settings) {
		Setting_Load(setting, ini, owner);
	}
}


// The renderer starts before Settings_Load runs, so the settings it needs are read here first.
template<typename Owner>
void Settings_Load_Before_Renderer(std::span<Setting<Owner> const> settings, INIClass const & ini, std::type_identity_t<Owner> & owner)
{
	for (Setting<Owner> const & setting : settings) {
		if (setting.BeforeRenderer) {
			Setting_Load(setting, ini, owner);
		}
	}
}


// Writes the keys in table order. INIClass moves a key it writes to the end of its section, so this order is the file's order.
template<typename Owner>
void Settings_Save(std::span<Setting<Owner> const> settings, INIClass & ini, std::type_identity_t<Owner> const & owner)
{
	for (Setting<Owner> const & setting : settings) {
		Setting_Save(setting, ini, owner);
	}
}
