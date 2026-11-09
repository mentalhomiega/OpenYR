/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ui/uisettingrows.h"

// windowsx.h defines macros with the names of these RmlUi Element methods.
#ifdef GetFirstChild
#undef GetFirstChild
#undef GetNextSibling
#endif

#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Core/DataTypeRegister.h>

// Registers UISettingRow in the model and binds the rows under the name given. The markup in ui/graphics.rml lists them.
bool UI_Rml_Setting_Rows_Bind(Rml::DataModelConstructor & model, char const * name, std::vector<UISettingRow> & rows);
