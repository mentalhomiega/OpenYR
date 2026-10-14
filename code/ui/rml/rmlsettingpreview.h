/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ui/uisettingpreview.h"

// windowsx.h defines macros with the names of these RmlUi Element methods.
#ifdef GetFirstChild
#undef GetFirstChild
#undef GetNextSibling
#endif

#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Core/DataTypeRegister.h>

/*
 * The setting-preview markup, which a page writes as in ui/display.rml, draws a mock from the model
 * variable the name gives. It reads preview.caption, and for each entry of preview.shapes it makes an
 * element with that shape's classes, left, top, width and height. ui/setting-preview.rcss places the
 * elements. A page holds one such variable per preview.
 */

// Registers the types of a mock screen in the model and binds the screen under the name given.
bool UI_Rml_Setting_Preview_Bind(Rml::DataModelConstructor & model, char const * name, UISettingPreviewScreen & screen);
