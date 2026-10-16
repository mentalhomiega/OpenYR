/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Fuzzes the launch file (spawn.ini) reader. The CnCNet client writes this file, but a player
// can edit it and a match host can influence its seats, so the reader must survive any text
// and leave the seat tables inside their bounds.

#include "fuzz.h"
#include "fuzzio.h"

#include "house.hh"
#include "ini.h"
#include "spawnerconfig.h"

#include <string>


namespace
{

using namespace Fuzz;


std::vector<Input> Seeds(void)
{
	std::vector<Input> seeds;

	seeds.push_back({"empty settings", From_String("[Settings]\n")});

	seeds.push_back({"resume", From_String(
		"[Settings]\nScenario=spawnmap.ini\nSaveGameName=SAVEGAME.001\nLoadSaveGame=Yes\n"
		"SidebarHack=True\nCustomLoadScreen=Resources/l600s01.pcx\nFirestorm=No\nGameSpeed=1\n")});

	seeds.push_back({"campaign", From_String(
		"[Settings]\nScenario=spawnmap.ini\nCampaignID=-1\nGameSpeed=1\nFirestorm=False\n"
		"IsSinglePlayer=Yes\nSide=0\nBuildOffAlly=False\nDifficultyModeHuman=0\nDifficultyModeComputer=2\n")});

	seeds.push_back({"skirmish", From_String(
		"[Settings]\nScenario=spawnmap.ini\nName=Commander\nSide=1\nColor=4\nColorRGB=255,128,0\n"
		"Port=1234\nAIPlayers=2\nCredits=5000\nShortGame=Yes\nNextSkirmishAutoSaveId=5\n"
		"[HouseColors]\nMulti2=2\nMulti3=7\n"
		"[HouseCountries]\nMulti2=0\nMulti3=1\n"
		"[HouseHandicaps]\nMulti2=2\nMulti3=0\n"
		"[SpawnLocations]\nMulti1=3\nMulti2=9\nMulti3=-2\n"
		"[Multi1_Alliances]\nHouseAllyOne=1\n")});

	seeds.push_back({"network", From_String(
		"[Settings]\nScenario=spawnmap.ini\nName=Second\nSide=1\nColor=5\nColorRGB=0,64,255\nPort=50000\nHost=No\n"
		"[Other1]\nName=First\nSide=0\nColor=1\nColorRGB=200,0,0\nIp=10.0.0.7\nPort=50001\n"
		"[Other2]\nName=Third\nSide=2\nColor=3\nIp=10.0.0.9\nPort=50002\nIsSpectator=Yes\n"
		"[Tunnel]\nIp=88.99.11.22\nPort=50010\n"
		"[IsSpectator]\nMulti1=Yes\n"
		"[Multi2_Alliances]\nHouseAllyOne=0\nHouseAllyTwo=2\n")});

	seeds.push_back({"timing", From_String(
		"[Settings]\nConnTimeout=1800\nReconnectTimeout=-5\nTimingPolicy=Adaptive\nSyncLog=Yes\n"
		"BindAddress=127.0.0.1\nGlobalFlag0=1\nTechLevel=7\nGameMode=2\nAIDifficulty=9\n")});

	seeds.push_back({"smallest numbers", From_String(
		"[Settings]\nCampaignID=-2147483648\nAutoSaveInterval=-2147483648\nAutoSaveGame=-2147483648\nNextSPAutoSaveId=-2147483648\nNextSkirmishAutoSaveId=-2147483648\nCredits=-2147483648\nGameSpeed=-2147483648\nUnitCount=-2147483648\nAIPlayers=-2147483648\nAIDifficulty=-2147483648\nGameMode=-2147483648\nSeed=-2147483648\nTechLevel=-2147483648\nDifficultyModeHuman=-2147483648\nDifficultyModeComputer=-2147483648\nPort=-2147483648\nConnTimeout=-2147483648\nReconnectTimeout=-2147483648\nColor=-2147483648\nSide=-2147483648\n"
		"[Other1]\nName=Other\nColor=-2147483648\nSide=-2147483648\nPort=-2147483648\nIp=1.2.3.4\n"
		"[Tunnel]\nIp=1.2.3.4\nPort=-2147483648\n"
		"[Multi1_Alliances]\nHouseAllyOne=-2147483648\n")});

	seeds.push_back({"largest numbers", From_String(
		"[Settings]\nCampaignID=2147483647\nAutoSaveInterval=2147483647\nAutoSaveGame=2147483647\nNextSPAutoSaveId=2147483647\nNextSkirmishAutoSaveId=2147483647\nCredits=2147483647\nGameSpeed=2147483647\nUnitCount=2147483647\nAIPlayers=2147483647\nAIDifficulty=2147483647\nGameMode=2147483647\nSeed=2147483647\nTechLevel=2147483647\nDifficultyModeHuman=2147483647\nDifficultyModeComputer=2147483647\nPort=2147483647\nConnTimeout=2147483647\nReconnectTimeout=2147483647\nColor=2147483647\nSide=2147483647\n"
		"[Other1]\nName=Other\nColor=2147483647\nSide=2147483647\nPort=2147483647\nIp=999.1.1.1\n"
		"[SpawnLocations]\nMulti1=2147483647\nMulti2=-2147483648\n"
		"[HouseHandicaps]\nMulti2=2147483647\n"
		"[Multi1_Alliances]\nHouseAllyOne=2147483647\n")});

	return(seeds);
}


// Inputs that crashed the reader. Each stays as it was found.
std::vector<Input> Regressions(void)
{
	std::vector<Input> inputs;

	// The autosave number was reduced by one with no room for the smallest int to go lower.
	inputs.push_back({"smallest autosave number", From_String("[Settings]\nNextSPAutoSaveId=-2147483648\nNextSkirmishAutoSaveId=-2147483648\n")});

	return(inputs);
}


std::vector<std::string> Dictionary(void)
{
	std::vector<std::string> tokens = {
		"[Settings]", "[Other1]", "[Other7]", "[Tunnel]", "[HouseColors]", "[HouseCountries]",
		"[HouseHandicaps]", "[SpawnLocations]", "[IsSpectator]", "[Multi1_Alliances]",
		"[Multi8_Alliances]", "Multi1=", "Multi8=", "Multi9=", "HouseAllyOne=", "HouseAllyEight=",
		"Name=", "Color=", "ColorRGB=", "Side=", "Port=", "Ip=", "Host=", "AIPlayers=",
		"AIDifficulty=", "Scenario=", "SaveGameName=", "LoadSaveGame=", "IsSinglePlayer=",
		"ConnTimeout=", "ReconnectTimeout=", "NextSPAutoSaveId=", "NextSkirmishAutoSaveId=",
		"AutoSaveGame=", "-2147483648", "2147483647", "-1", "255,255,255", "256,0,0", "-1,0,0",
		"1,2", "1,2,3,4", "../", "..\\", "C:\\", "\xC3\xA9", "\xFF\xFE"};
	return(tokens);
}


void Run(Bytes const & input)
{
	INIClass ini;
	With_Straw(input, [&](Straw & straw) {ini.Load(straw);});

	SpawnerConfigClass config;
	config.Read_INI(ini);

	Require(config.HumanCount >= 0 && config.HumanCount <= SpawnerConfigClass::SLOT_COUNT, "HumanCount is outside the seat table");
	Require(config.LocalSlot >= 0 && config.LocalSlot < SpawnerConfigClass::SLOT_COUNT, "LocalSlot is outside the seat table");
	Require(config.ConnTimeout >= SpawnerConfigClass::TIMEOUT_MIN && config.ConnTimeout <= SpawnerConfigClass::TIMEOUT_MAX, "ConnTimeout is outside its bounds");
	Require(config.ReconnectTimeout >= SpawnerConfigClass::TIMEOUT_MIN && config.ReconnectTimeout <= SpawnerConfigClass::TIMEOUT_MAX, "ReconnectTimeout is outside its bounds");

	for (SpawnerConfigClass::SlotType const & slot : config.Slots) {
		Require(slot.StartingPosition >= -1 && slot.StartingPosition < SpawnerConfigClass::SLOT_COUNT, "a start position is outside the seat table");
		Require(slot.Name.find('\0') == std::string::npos, "a seat name holds a NUL");
		Require(slot.Name.size() < (std::size_t)HOUSE_NAME_MAX, "a seat name is longer than a house name");
	}

	// The identity is a pure function of the reading.
	Require(config.Session_Identity_CRC() == config.Session_Identity_CRC(), "the session identity changes between calls");

	(void)config.Launch_Type();
	for (int handicap : {-5, -1, 0, 1, 2, 3, 6, 7, 1000}) {
		int const played = SpawnerConfigClass::Playable_Handicap(handicap);
		Require(played >= -1, "Playable_Handicap returned less than -1");
	}

	// Judge the reading against table sizes that are too small, ordinary and too large.
	int const counts[][2] = {{0, 0}, {1, 1}, {9, 8}, {17, 20}, {1000, 1000}};
	for (auto const & count : counts) {
		std::string fault;
		bool const playable = config.Is_Playable(count[0], count[1], fault);
		Require(!(playable && !fault.empty()), "Is_Playable left a fault with a playable reading");
	}

	// A file read over an earlier reading gives the same match.
	int const first = config.Session_Identity_CRC();
	config.Read_INI(ini);
	Require(config.Session_Identity_CRC() == first, "reading the same file twice changes the match");
}

}


namespace Fuzz
{

Target const & Get_Target(void)
{
	static Target const target = {"spawn", 4096, 10000, Seeds, Regressions, Dictionary, Run};
	return(target);
}

}
