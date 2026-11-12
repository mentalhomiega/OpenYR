/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/rml/rmlsettingrows.h"


bool UI_Rml_Setting_Rows_Bind(Rml::DataModelConstructor & model, char const * name, std::vector<UISettingRow> & rows)
{
	Rml::StructHandle<UISettingRow> row = model.RegisterStruct<UISettingRow>();
	if (!row || !model.RegisterArray<std::vector<std::string>>() || !model.RegisterArray<std::vector<UISettingRow>>()) {
		return(false);
	}
	row.RegisterMember("kind", &UISettingRow::KindName);
	row.RegisterMember("intent", &UISettingRow::Intent);
	row.RegisterMember("id", &UISettingRow::ElementId);
	row.RegisterMember("rowid", &UISettingRow::RowId);
	row.RegisterMember("label", &UISettingRow::Label);
	row.RegisterMember("tip", &UISettingRow::Tip);
	row.RegisterMember("classes", &UISettingRow::Classes);
	row.RegisterMember("valuetext", &UISettingRow::ValueText);
	row.RegisterMember("on", &UISettingRow::On);
	row.RegisterMember("disabled", &UISettingRow::Disabled);
	row.RegisterMember("value", &UISettingRow::Value);
	row.RegisterMember("min", &UISettingRow::Min);
	row.RegisterMember("max", &UISettingRow::Max);
	row.RegisterMember("items", &UISettingRow::Items);
	row.RegisterMember("note", &UISettingRow::Note);
	return(model.Bind(name, &rows));
}
