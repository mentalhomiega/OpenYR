/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "loadprogress.h"

#include "autotest.h"
#include "globals.h"
#include "goptions.h"
#include "ui/screens/waitbox/uiwaitbox.h"

#include <memory>


static std::unique_ptr<UIWaitBoxClass> Notice;
static int Depth = 0;
static int Muted = 0;


void Load_Progress_Begin(char const * text, bool backdrop)
{
	if (Depth++ > 0 || Options.IsClassicMenus) {
		return;
	}

	Notice = std::make_unique<UIWaitBoxClass>();
	if (!Notice->Show(text, true, !backdrop)) {
		Notice.reset();
	}
}


void Load_Progress_End(void)
{
	if (Depth == 0 || --Depth > 0) {
		return;
	}

	Notice.reset();
}


void Load_Progress_Step(char const * text, double fraction)
{
	if (Notice == nullptr || Muted > 0) {
		return;
	}

	Notice->Set_Step(text, fraction);
	AutoTest_Load_Progress_Shown(text, fraction);
}


LoadProgressMuteClass::LoadProgressMuteClass(bool mute) :
	Muting(mute)
{
	if (Muting) {
		Muted++;
	}
}


LoadProgressMuteClass::~LoadProgressMuteClass(void)
{
	if (Muting) {
		Muted--;
	}
}


LoadProgressScopeClass::LoadProgressScopeClass(char const * text)
{
	Load_Progress_Begin(text);
}


LoadProgressScopeClass::~LoadProgressScopeClass(void)
{
	Load_Progress_End();
}
