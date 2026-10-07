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
#include "_rules.h"
#include "cdfile.h"
#include "ccfile.h"
#include "ccini.h"
#include "deploymentconfig.h"
#include "gamedirs.h"
#include "mods.h"
#include "msgbox.h"
#include "ui/screens/mods/uimods.h"
#include "ui/uienginehost.h"
#include "ui/uisettings.h"
#include "ui/uiview.h"


namespace
{

class UIModsEngineServiceClass : public UIModsServiceClass
{
	public:
		virtual bool Save(std::string const & list) override
		{
			std::string const before = Configured_Mod_List(ConfigINI, "");
			bool const present = Has_Mod_List(ConfigINI);
			Put_Mod_List(ConfigINI, list);

			CCFileClass file(DeploymentConfig.SettingsFile.c_str());
			if (ConfigINI.Save(file, false) > 0) {
				return(true);
			}

			if (present) {
				Put_Mod_List(ConfigINI, before);
			} else {
				Clear_Mod_List(ConfigINI);
			}
			return(false);
		}

		virtual std::string File_Name(void) override
		{
			char const * user = CDFileClass::User_Path();
			return(std::string(user != NULL ? user : "") + DeploymentConfig.SettingsFile);
		}
};

UIModsEngineServiceClass _Service;

}


UIModsServiceClass & UI_Mods_Service(void)
{
	return(_Service);
}


/// <summary>
/// Shows the Mods screen over the player's list, as last saved in their settings file or else
/// the deployment's, and the mods the command line names. When OK saves a changed list, a
/// message says it takes effect after a restart.
/// </summary>
void UI_Mods_Dialog(void)
{
	UIModsPresenterClass presenter(UI_Mods_Service(), Mod_Choices(Configured_Mod_List(ConfigINI, DeploymentConfig.Mods.c_str()).c_str(), Data_Directory()));
	UI_Settings_Join(presenter, UI_TAB_MODS);
	std::unique_ptr<UIViewClass> view = UI_Mods_View(presenter);

	UIResult const result = UI_Run_Modal(*view);
	UI_Settings_Leave(presenter);
	if (result != UI_RESULT_ACCEPTED || !presenter.Saved) {
		return;
	}

	WWMessageBox().Process("The new mod list takes effect when you restart the game.", TXT_OK);
}
