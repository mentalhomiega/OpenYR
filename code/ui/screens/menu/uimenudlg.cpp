/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_surface.h"
#include "_ui.h"
#include "globals.h"
#include "goptions.h"
#include "init.h"
#include "surface.h"
#include "ui/screens/menu/uimenu.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uiview.h"


// The menu styles place the menus themselves, so the menu keeps the position its sheet gives it.
// The modern title menus stand at the left edge, where the slide-out from the middle would clip them.
void UI_Menu_Place(UIMenuState & state)
{
	state.Top = -1;
	state.Slide = Options.IsClassicMenus;
}


int UI_Menu_Dialog(UIMenuState const & state, int nothing, std::function<bool(void)> const & hook)
{
	UIMenuPresenterClass presenter(state);
	std::unique_ptr<UIViewClass> view = UI_Menu_View(presenter);

	UIResult result = UIShell.Run_Modal(*view, [&hook]() {
		bool ended = UI_Service_Game();
		Title_Screen_Restore();
		if (hook && hook()) {
			ended = true;
		}
		return(ended);
	});

	return((result == UI_RESULT_ACCEPTED) ? presenter.Choice : nothing);
}
