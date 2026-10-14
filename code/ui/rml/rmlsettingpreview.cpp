/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/rml/rmlsettingpreview.h"

bool UI_Rml_Setting_Preview_Bind(Rml::DataModelConstructor & model, char const * name, UISettingPreviewScreen & screen)
{
	Rml::StructHandle<UISettingPreviewShape> shape = model.RegisterStruct<UISettingPreviewShape>();
	if (!shape) {
		return(false);
	}
	shape.RegisterMember("classes", &UISettingPreviewShape::Classes);
	shape.RegisterMember("left", &UISettingPreviewShape::Left);
	shape.RegisterMember("top", &UISettingPreviewShape::Top);
	shape.RegisterMember("width", &UISettingPreviewShape::Width);
	shape.RegisterMember("height", &UISettingPreviewShape::Height);

	Rml::StructHandle<UISettingPreviewScreen> mock = model.RegisterStruct<UISettingPreviewScreen>();
	if (!mock || !model.RegisterArray<std::vector<UISettingPreviewShape>>()) {
		return(false);
	}
	mock.RegisterMember("caption", &UISettingPreviewScreen::Caption);
	mock.RegisterMember("shapes", &UISettingPreviewScreen::Shapes);

	return(model.Bind(name, &screen));
}
