/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "campaign.h"

#include "campaignname.hh"
#include "ccfile.h"
#include "ccini.h"
#include "crc.h"
#include "globals.h"
#include "savestream.h"
#include "scenario.h"
#include "sun.h"
#include "tracker.h"
#include "vector.h"

#include <cassert>
#include <cstdio>
#include <string>


/// <summary>
/// Creates a campaign and adds it to the campaign list.
/// The campaign starts out with the display description borrowed from its given
/// name; the rules will normally supply a proper one later.
/// </summary>
/// <param name="name">The name of the campaign as it appears in the rules.</param>
CampaignClass::CampaignClass(char const * name) :
	BASECLASS(name),
	CDNumber(-1),
	FinalMovie(VQ_NONE),
	RequiredAddon(0),
	DebugOnly(false)
{
	Campaigns.Add(this);

	ScenarioName[0] = '\0';
	strcpy(Description, GivenName);
}


/// <summary>
/// Destroys this campaign and removes it from the game.
/// Any object still pointing at this campaign is detached from it before the
/// campaign leaves the master campaign list.
/// </summary>
CampaignClass::~CampaignClass(void)
{
	Detach_This_From_All(this, true);
	Campaigns.Delete(this);
}


/// <summary>
/// Converts a campaign name into a campaign identifier.
/// </summary>
/// <param name="name">The campaign name to search for. Case is ignored.</param>
/// <returns>Returns with the campaign found, or CAMPAIGN_NONE if there is no
/// such campaign.</returns>
CampaignType CampaignClass::From_Name(char const * name)
{
	assert(name != NULL);

	for (int index = 0; index < Campaigns.Count(); index++) {
		if (stricmp(Campaigns[index]->IniName, name) == 0) {
			return(CampaignType(index));
		}
	}
	return(CAMPAIGN_NONE);
}


/// <summary>
/// Fetches this campaign's settings from the rules.
/// This routine picks up the disc the campaign lives on, the scenario it starts
/// with, the movie that closes it out, the expansion it requires, and whether it is
/// kept for testing only.
/// </summary>
/// <param name="ini">The rules database to read the campaign settings from.</param>
/// <returns>bool; Was the campaign found and read?</returns>
bool CampaignClass::Read_INI(CCINIClass const & ini)
{
	if (BASECLASS::Read_INI(ini)) {
		CDNumber = ini.Get_Int(IniName, "CD", CDNumber);
		FinalMovie = ini.Get_VQType(IniName, "FinalMovie", FinalMovie);
		if (ini.Is_Present(IniName, "Scenario")) {
			std::string archive;
			std::string file;
			Campaign_Split_Scenario(ini.Get_String(IniName, "Scenario"), archive, file);
			ScenarioArchive = archive;
			std::snprintf(ScenarioName, sizeof(ScenarioName), "%s", file.c_str());
			strupr(ScenarioName);
		}
		ini.Get_String(IniName, "Description", Description, Description, sizeof(Description));
		RequiredAddon = ini.Get_Int(IniName, "RequiredAddon", RequiredAddon);
		DebugOnly = ini.Get_Bool(IniName, "DebugOnly", DebugOnly);
		return(true);
	}
	return(false);
}


/// <summary>
/// Creates the campaign list from the rules.
/// This routine walks the Battles section and gives every campaign named there a
/// CampaignClass object, creating one where none exists yet and then letting each
/// campaign fetch its own settings.
/// </summary>
/// <param name="ini">The rules database to read the campaign list from.</param>
void Read_Battle_INI(CCINIClass const & ini)
{
	char name[32];

	int count = ini.Entry_Count("Battles");

	for (int index = 0; index < count; index++) {

		ini.Get_String("Battles", ini.Get_Entry("Battles", index), "", name, sizeof(name));

		CampaignType ctype = CampaignClass::From_Name(name);
		CampaignClass * campaign;

		if (ctype == CAMPAIGN_NONE) {
			campaign = new CampaignClass(name);
		} else {
			campaign = Campaigns[ctype];
		}

		campaign->Read_INI(ini);
	}
}


/// <summary>
/// Asks whether the running mission belongs to a campaign whose Scenario= has the RA2-> prefix.
/// Such a mission is written against Red Alert 2's lists, which Yuri's Revenge's lists differ from in the
/// places where their numbers matter.
/// </summary>
bool Scenario_Is_From_RA2(void)
{
	return(Scen != NULL && Scen->Campaign >= 0 && Scen->Campaign < Campaigns.Count() && Campaigns[Scen->Campaign] != NULL
		&& Campaigns[Scen->Campaign]->ScenarioArchive == "RA2");
}


/// <summary>
/// Finds the mission that follows the running Red Alert 2 mission: the next entry of the campaign list, when that entry
/// is a Red Alert 2 mission whose map exists. A header or movie row, the end of the list, or a missing map ends the
/// campaign.
/// </summary>
/// <returns>CampaignType; the entry to play next, or CAMPAIGN_NONE when the campaign ends.</returns>
CampaignType Campaign_Next_RA2_Mission(void)
{
	if (!Scenario_Is_From_RA2() || Scen->Campaign + 1 >= Campaigns.Count()) {
		return(CAMPAIGN_NONE);
	}

	CampaignClass const * next = Campaigns[Scen->Campaign + 1];
	if (next == NULL || next->ScenarioArchive != "RA2" || !Campaign_Is_Mission_File(next->ScenarioName)
		|| !CCFileClass(next->ScenarioName).Is_Available()) {
		return(CAMPAIGN_NONE);
	}
	return(CampaignType(Scen->Campaign + 1));
}


ClassID CampaignClass::Class_ID(void) const
{
	return(ClassID_CampaignClass);
}


/// <summary>
/// Submits this campaign to the CRC engine.
/// This routine is used by the multiplayer sync check to prove that every machine
/// is playing with the same campaign data.
/// </summary>
void CampaignClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(CDNumber);
	crc(ScenarioName);
	crc(FinalMovie);
	crc(Description);
	crc(RequiredAddon);
}


/// <summary>
/// Lists the members this campaign carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void CampaignClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(CDNumber);
	stream.Serialize(ScenarioName);
	stream.Serialize(FinalMovie);
	stream.Serialize(Description);
	stream.Serialize(RequiredAddon);
}


/// <summary>
/// Fetches the RTTI identifier of this object.
/// </summary>
/// <returns>Returns with RTTI_CAMPAIGN.</returns>
RTTIType CampaignClass::Fetch_RTTI(void) const
{
	return(RTTI_CAMPAIGN);
}
