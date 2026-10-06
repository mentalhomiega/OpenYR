/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_deploymentconfig.h"
#include "deploymentconfig.h"
#include "gamedirs.h"
#include "mods.h"
#include "msgbox.h"
#include "ui/screens/mods/uimods.h"
#include "ui/uienginehost.h"
#include "ui/uiview.h"


namespace
{

class UIModsEngineServiceClass : public UIModsServiceClass
{
	public:
		virtual bool Save(std::string const & list) override
		{
			return(DeploymentConfig.Write_Mods(Data_Directory().c_str(), list));
		}

		virtual std::string File_Name(void) override
		{
			return(DeploymentConfig.Mods_File_Name(Data_Directory().c_str()));
		}
};

UIModsEngineServiceClass _Service;

}


UIModsServiceClass & UI_Mods_Service(void)
{
	return(_Service);
}


/// <summary>
/// Shows the Mods screen over the list in OPENTS.INI, as last saved, and the mods the command
/// line names. When OK saves a changed list, a message says it takes effect after a restart.
/// </summary>
void UI_Mods_Dialog(void)
{
	UIModsPresenterClass presenter(UI_Mods_Service(), Mod_Choices(DeploymentConfig.Mods.c_str(), Data_Directory()));
	std::unique_ptr<UIViewClass> view = UI_Mods_View(presenter);

	if (UI_Run_Modal(*view) != UI_RESULT_ACCEPTED || !presenter.Saved) {
		return;
	}

	WWMessageBox().Process("The new mod list takes effect when you restart the game.", TXT_OK);
}
