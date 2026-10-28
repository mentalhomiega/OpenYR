/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "spawnerconfig.h"

#include "crc.h"
#include "diff.hh"
#include "ini.h"
#include "playercolor.h"
#include "rgb.h"
#include "utf8.h"

#include <algorithm>
#include <climits>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <vector>


namespace {

// The match's options, which also describe this machine's own seat.
char const * const SETTINGS = "Settings";


/// <summary>
/// Reads a seat's ColorRGB entry, three values from 0 to 255 separated by commas, as a free color.
/// Any other text gives zero, so the seat keeps the stock color its Color entry names.
/// </summary>
unsigned int Read_Free_Color(INIClass const & ini, char const * section)
{
	std::string const text = ini.Get_String(section, "ColorRGB");
	int red = -1;
	int green = -1;
	int blue = -1;
	char extra = 0;
	if (std::sscanf(text.c_str(), "%d,%d,%d%c", &red, &green, &blue, &extra) != 3) {
		return(0);
	}
	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255) {
		return(0);
	}
	return(Player_Color_From_RGB(RGBClass((unsigned char)red, (unsigned char)green, (unsigned char)blue)));
}


/// <summary>Reads a string entry, or the fallback when it is empty.</summary>
std::string Read_Text(INIClass const & ini, char const * section, char const * entry, std::string const & fallback)
{
	std::string text = ini.Get_String(section, entry);
	return(text.empty() ? fallback : text);
}


/// <summary>
/// Brings a wait written in a launch file within the bounds the game accepts.
/// </summary>
int Clamp_Timeout(int ticks)
{
	return(std::clamp(ticks, SpawnerConfigClass::TIMEOUT_MIN, SpawnerConfigClass::TIMEOUT_MAX));
}


/// <summary>
/// Reads one of the eight numbered entries a section names its seats by.
/// </summary>
/// <returns>The value written for that seat.</returns>
int Read_Slot_Int(INIClass const & ini, char const * section, int slot, int fallback)
{
	std::string entry = "Multi" + std::to_string(slot + 1);
	return(ini.Get_Int(section, entry.c_str(), fallback));
}


/// <summary>
/// Turns a number the client counts from one into one the game counts from zero. The smallest
/// int has no smaller neighbor and stays as it is.
/// </summary>
int Count_From_Zero(int number)
{
	return(number == INT_MIN ? number : number - 1);
}


/// <summary>Is this a dotted IPv4 address other than 0.0.0.0?</summary>
bool Is_Address(std::string const & text)
{
	unsigned quad[4] = {};
	char tail = '\0';

	if (std::sscanf(text.c_str(), "%u.%u.%u.%u%c", &quad[0], &quad[1], &quad[2], &quad[3], &tail) != 4) {
		return(false);
	}

	for (unsigned part : quad) {
		if (part > 255) {
			return(false);
		}
	}

	return(quad[0] != 0 || quad[1] != 0 || quad[2] != 0 || quad[3] != 0);
}


/// <summary>Accepts a tunnel number as the client writes it: nonzero, within sixteen bits either side of zero.</summary>
static bool Is_Tunnel_Number(int value)
{
	return(value != 0 && value >= -65535 && value <= 65535);
}


/// <summary>
/// Reads a bool the way gamemd's launch-file reader does: the key matches without regard to
/// case, so "Superweapons" and "SuperWeapons" are the same key. INIClass looks names up exactly,
/// so the entries are scanned for the first one whose name matches.
/// </summary>
/// <param name="ini">The launch file.</param>
/// <param name="section">The section to read from.</param>
/// <param name="key">The key the spawner knows the value by.</param>
/// <param name="defvalue">The value when no entry of that name holds a boolean.</param>
static bool Spawn_Bool(INIClass const & ini, char const * section, char const * key, bool defvalue)
{
	int const count = ini.Entry_Count(section);
	for (int index = 0; index < count; index++) {
		char const * entry = ini.Get_Entry(section, index);
		if (entry != NULL && _stricmp(entry, key) == 0) {
			return(ini.Get_Bool(section, entry, defvalue));
		}
	}
	return(defvalue);
}


/// <summary>
/// Names the fault that refuses a launch.
/// </summary>
/// <param name="format">A printf style description of the fault.</param>
/// <returns>false, so a caller can name a fault and refuse in one statement.</returns>
bool Fault(std::string & fault, char const * format, ...)
{
	char buffer[256];

	va_list args;
	va_start(args, format);
	std::vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	fault = buffer;
	return(false);
}

}


/// <summary>
/// Reads the match's seats and sorts them into the order their houses are created in, which
/// is the order everything naming a seat by position means. A seat is human because the file
/// wrote a section for it.
/// </summary>
void SpawnerConfigClass::Read_Slots(INIClass const & ini)
{
	std::array<SlotType, SLOT_COUNT> staging;

	for (int index = 0; index < SLOT_COUNT; index++) {
		std::string section = index == 0 ? SETTINGS : "Other" + std::to_string(index);

		SlotType & slot = staging[index];
		if (ini.Section_Present(section.c_str())) {
			slot.Occupancy = OccupancyType::Human;
			// A seat is judged and ordered by the name the game keeps, the same on every machine.
			std::string name = Read_Text(ini, section.c_str(), "Name", "");
			slot.Name = name.substr(0, UTF8::Boundary_Before(name.c_str(), HOUSE_NAME_MAX - 1));
			slot.Color = ini.Get_Int(section.c_str(), "Color", -1);
			slot.CustomColor = Read_Free_Color(ini, section.c_str());
			slot.Country = ini.Get_Int(section.c_str(), "Side", -1);
			slot.IsSpectator = Spawn_Bool(ini, section.c_str(), "IsSpectator", false);
			slot.Address = Read_Text(ini, section.c_str(), "Ip", slot.Address);
			slot.Port = ini.Get_Int(section.c_str(), "Port", 0);
		} else {
			slot.Color = Read_Slot_Int(ini, "HouseColors", index, -1);
			slot.Country = Read_Slot_Int(ini, "HouseCountries", index, -1);
			slot.Handicap = Read_Slot_Int(ini, "HouseHandicaps", index, -1);
		}
	}

	// Sorting by color makes a seat's index the house it becomes. Each machine writes itself
	// first, so a name breaks a color tie rather than file order.
	std::vector<int> humans;
	std::vector<int> rest;
	for (int index = 0; index < SLOT_COUNT; index++) {
		(staging[index].Occupancy == OccupancyType::Human ? humans : rest).push_back(index);
	}
	std::stable_sort(humans.begin(), humans.end(), [&staging](int left, int right) {
		if (staging[left].Color != staging[right].Color) {
			return(staging[left].Color < staging[right].Color);
		}
		return(_stricmp(staging[left].Name.c_str(), staging[right].Name.c_str()) < 0);
	});

	HumanCount = (int)humans.size();
	LocalSlot = 0;

	int filled = 0;
	for (int index : humans) {
		if (index == 0) {
			LocalSlot = filled;
		}
		Slots[filled++] = staging[index];
	}

	// A seat no section claimed is a computer player, up to AIPlayers of them.
	for (int index : rest) {
		SlotType & slot = Slots[filled];
		slot = staging[index];
		slot.Occupancy = (filled - HumanCount) < AIPlayers ? OccupancyType::Computer : OccupancyType::Empty;
		filled++;
	}

	// The alliance sections name seats in sorted order, so they are read after the sort.
	static char const * const _ordinals[SLOT_COUNT] = {
		"HouseAllyOne", "HouseAllyTwo", "HouseAllyThree", "HouseAllyFour",
		"HouseAllyFive", "HouseAllySix", "HouseAllySeven", "HouseAllyEight"
	};

	for (int index = 0; index < SLOT_COUNT; index++) {
		SlotType & slot = Slots[index];

		std::string entry = "Multi" + std::to_string(index + 1);
		slot.IsSpectator = slot.IsSpectator || Spawn_Bool(ini, "IsSpectator", entry.c_str(), false);
		slot.StartingPosition = Read_Slot_Int(ini, "SpawnLocations", index, -1);

		// A start position out of range is left to the game, like no position at all.
		if (slot.StartingPosition < -1 || slot.StartingPosition >= SLOT_COUNT) {
			slot.StartingPosition = -1;
		}

		std::string section = "Multi" + std::to_string(index + 1) + "_Alliances";
		if (!ini.Section_Present(section.c_str())) {
			continue;
		}

		for (int ally = 0; ally < SLOT_COUNT; ally++) {
			slot.Alliances[ally] = ini.Get_Int(section.c_str(), _ordinals[ally], -1);
		}
	}
}


/// <summary>
/// What kind of game this file asks for. A resume answers by itself, because the save carries
/// the type, the options and the houses.
/// </summary>
SpawnerConfigClass::LaunchType SpawnerConfigClass::Launch_Type(void) const
{
	if (LoadSaveGame) {
		return(LaunchType::Resume);
	}
	if (IsCampaign) {
		return(LaunchType::Campaign);
	}
	if (HumanCount > 1) {
		return(LaunchType::Multiplayer);
	}
	return(LaunchType::Skirmish);
}


/// <summary>
/// The identity of the match this file asks for. It covers every value the course of the
/// match depends on and nothing merely displayed, so two machines handed the same match
/// agree. The version comes first: one file read two ways is not one match.
/// </summary>
int SpawnerConfigClass::Session_Identity_CRC(void) const
{
	CRCEngine crc;

	crc(SCHEMA_VERSION);

	crc(ScenarioName.c_str());
	crc(IsCampaign);
	crc(CampaignID);
	crc(CampaignDifficulty);
	crc(CampaignCDifficulty);
	crc(LoadSaveGame);
	crc(SaveGameName.c_str());

	crc(Bases);
	crc(Credits);
	crc(BridgeDestroy);
	crc(Crates);
	crc(ShortGame);
	crc(BuildOffAlly);
	crc(GameSpeed);
	crc(MultiEngineer);
	crc(UnitCount);
	crc(AIPlayers);
	crc(AIDifficulty);
	crc(AlliesAllowed);
	crc(GameMode);
	crc(HarvesterTruce);
	crc(FogOfWar);
	crc(MCVRedeploy);
	crc(AutoDeployMCV);
	crc(Seed);
	crc(TechLevel);
	crc(Firestorm);
	crc(AttackNeutralUnits);
	crc(ScrapMetal);
	crc(CoachMode);
	crc(PlayMoviesInMultiplayer);

	crc(AutoSurrender);

	// ConnTimeout, ReconnectTimeout and TimingPolicy are each machine's own, so they are left out on purpose.
	// A host's send period and look-ahead are shared: machines that start with different ones would stall.
	// They count only when chosen, so a match that chose nothing keeps the identity it always had.
	if (TimingChoice.Is_Set()) {
		crc(static_cast<int>(TimingChoice.FrameSendRate));
		crc(static_cast<int>(TimingChoice.MaxAhead));
	}
	if (Protocol != 2) {
		crc(Protocol);
	}

	for (bool flag : GlobalFlags) {
		crc(flag);
	}

	for (SlotType const & slot : Slots) {
		crc(static_cast<int>(slot.Occupancy));
		crc(slot.Color);
		crc(slot.Country);
		crc(slot.Handicap);
		crc(slot.IsSpectator);
		crc(slot.StartingPosition);

		for (int ally : slot.Alliances) {
			crc(ally);
		}
	}

	return(crc());
}


/// <summary>
/// The difficulty a seat is played at. A client may ask for an easier opponent than the game
/// has, and any such request comes to the easiest one it does have.
/// </summary>
/// <returns>The difficulty to play the seat at, or -1 for the session default.</returns>
int SpawnerConfigClass::Playable_Handicap(int asked)
{
	if (asked < 0) {
		return(-1);
	}
	// The rules' hardest table makes the easiest opponent, so an easier request lands there.
	if (asked > DIFF_HARD) {
		return(DIFF_HARD);
	}
	return(asked);
}


/// <summary>
/// Judges whether this reading describes a game that can be played. The country and color
/// counts are passed in because they come from the rules, which only a running game holds.
/// </summary>
/// <param name="fault">Where to leave the sentence describing the first fault found.</param>
/// <returns>bool; Can the game this file describes be played?</returns>
bool SpawnerConfigClass::Is_Playable(int countries, int colors, std::string & fault) const
{
	// A resumed network match is seated from the file, so the same rules hold.
	LaunchType kind = Launch_Type();
	bool multiplayer = kind == LaunchType::Multiplayer ||
		(kind == LaunchType::Resume && HumanCount > 1);

	if (kind != LaunchType::Campaign && HumanCount == 0) {
		return(Fault(fault, "The file seats nobody at this machine."));
	}

	if (AIDifficulty < 0 || AIDifficulty >= DIFF_COUNT) {
		return(Fault(fault, "The file plays the computer at difficulty %d, and there are %d.",
			AIDifficulty, DIFF_COUNT));
	}

	int free_seats = SLOT_COUNT - HumanCount;
	if (AIPlayers < 0 || AIPlayers > free_seats) {
		return(Fault(fault, "The file asks for %d computer players, and %d seats are left.",
			AIPlayers, free_seats));
	}

	// A match of watchers alone has nothing to watch. A resume leaves this to the save.
	if (kind != LaunchType::Campaign && kind != LaunchType::Resume && AIPlayers == 0) {
		bool plays = false;
		for (SlotType const & slot : Slots) {
			plays = plays || (slot.Occupancy == OccupancyType::Human && !slot.IsSpectator);
		}
		if (!plays) {
			return(Fault(fault, "The file seats nobody who plays: every seat watches and no computer plays."));
		}
	}

	// These reach the network as 16-bit values. A version 2 tunnel numbers machines across the
	// whole signed 16-bit range, so about half of its numbers arrive negative.
	if (multiplayer) {
		if (TunnelPort != 0) {
			if (TunnelPort < 1 || TunnelPort > 65535) {
				return(Fault(fault, "The tunnel is reached on port %d, which names no machine.",
					TunnelPort));
			}

			if (!Is_Address(TunnelAddress)) {
				return(Fault(fault, "The tunnel is reached at %s, which names no machine.",
					TunnelAddress.c_str()));
			}

			if (!Is_Tunnel_Number(TunnelId)) {
				return(Fault(fault, "The tunnel knows this machine as %d, which is not a tunnel number.",
					TunnelId));
			}
		} else if (ListenPort < 1 || ListenPort > 65535) {
			return(Fault(fault, "This machine listens on port %d, which is not a port.", ListenPort));
		} else if (!BindAddress.empty() && !Is_Address(BindAddress)) {
			return(Fault(fault, "This machine binds to %s, which is not a dotted IPv4 address.",
				BindAddress.c_str()));
		}
	}

	for (int index = 0; index < SLOT_COUNT; index++) {
		SlotType const & slot = Slots[index];
		if (slot.Occupancy == OccupancyType::Empty) {
			continue;
		}

		bool human = slot.Occupancy == OccupancyType::Human;

		// A computer seat may leave its country and color to the game; a person's seat names both.
		if ((human || slot.Country != -1) && (slot.Country < 0 || slot.Country >= countries)) {
			return(Fault(fault, "Seat %d is given country %d, and there are %d to choose from.",
				index + 1, slot.Country, countries));
		}

		// A computer seat may leave its color to the game; only a color the rules lack refuses.
		if ((human || slot.Color != -1) && (slot.Color < 0 || slot.Color >= colors)) {
			return(Fault(fault, "Seat %d is given color %d, and there are %d to choose from.",
				index + 1, slot.Color, colors));
		}

		if (slot.Handicap < -1 || slot.Handicap > 6) {
			return(Fault(fault, "Seat %d is given difficulty %d, which names none.",
				index + 1, slot.Handicap));
		}

		for (int ally : slot.Alliances) {
			if (ally < -1 || ally >= SLOT_COUNT ||
				(ally >= 0 && Slots[ally].Occupancy == OccupancyType::Empty)) {
				return(Fault(fault, "Seat %d is allied to seat %d, which the match does not hold.",
					index + 1, ally + 1));
			}
		}

		// The client's seat order cannot be rebuilt when two people share a name or a color, so
		// they would swap start positions and alliances.
		if (human && multiplayer) {
			if (slot.Name.empty()) {
				return(Fault(fault, "Seat %d is played by somebody the file does not name.", index + 1));
			}

			for (int other = 0; other < index; other++) {
				if (Slots[other].Occupancy != OccupancyType::Human) {
					continue;
				}

				if (_stricmp(Slots[other].Name.c_str(), slot.Name.c_str()) == 0) {
					return(Fault(fault, "Seats %d and %d are both played by %s.",
						other + 1, index + 1, slot.Name.c_str()));
				}

				if (Slots[other].Color == slot.Color) {
					return(Fault(fault, "Seats %d and %d are both given color %d.",
						other + 1, index + 1, slot.Color));
				}
			}

			// Through a tunnel the port carries the tunnel number, so every other seat needs one.
			if (index != LocalSlot) {
				if (TunnelPort != 0 && !Is_Tunnel_Number(slot.Port)) {
					return(Fault(fault, "The tunnel knows seat %d as %d, which is not a tunnel number.",
						index + 1, slot.Port));
				}

				if (TunnelPort == 0 && (slot.Port < 1 || slot.Port > 65535)) {
					return(Fault(fault, "Seat %d is reached on port %d, which names no machine.",
						index + 1, slot.Port));
				}

				if (TunnelPort == 0 && !Is_Address(slot.Address)) {
					return(Fault(fault, "Seat %d is reached at %s, which names no machine.",
						index + 1, slot.Address.c_str()));
				}
			}
		}
	}

	return(true);
}


/// <summary>
/// Reads what the client asked the game to launch. Reading cannot fail: an unwritten key has
/// a settled meaning, an unusable value keeps it, and an unknown key is passed over.
/// </summary>
void SpawnerConfigClass::Read_INI(INIClass const & ini)
{
	IsCampaign = Spawn_Bool(ini, SETTINGS, "IsSinglePlayer", IsCampaign);
	IsHost = Spawn_Bool(ini, SETTINGS, "Host", IsHost);
	CampaignID = ini.Get_Int(SETTINGS, "CampaignID", CampaignID);

	ScenarioName = Read_Text(ini, SETTINGS, "Scenario", ScenarioName);
	MapName = Read_Text(ini, SETTINGS, "UIMapName", MapName);

	LoadSaveGame = Spawn_Bool(ini, SETTINGS, "LoadSaveGame", LoadSaveGame);

	// Saves are opened by name in the saved-games folder, so a path is cut to its last part.
	SaveGameName = std::filesystem::path(Read_Text(ini, SETTINGS, "SaveGameName", SaveGameName)).filename().string();

	// AutoSaveGame wins when a file writes both keys; the CnCNet client writes AutoSaveInterval.
	AutoSaveInterval = ini.Get_Int(SETTINGS, "AutoSaveInterval", AutoSaveInterval);
	AutoSaveInterval = ini.Get_Int(SETTINGS, "AutoSaveGame", AutoSaveInterval);

	// The client counts automatic saves from one; the game counts from zero.
	NextCampaignAutoSave = Count_From_Zero(ini.Get_Int(SETTINGS, "NextSPAutoSaveId", 1));
	NextSkirmishAutoSave = Count_From_Zero(ini.Get_Int(SETTINGS, "NextSkirmishAutoSaveId", 1));

	Bases = Spawn_Bool(ini, SETTINGS, "Bases", Bases);
	Credits = ini.Get_Int(SETTINGS, "Credits", Credits);
	BridgeDestroy = Spawn_Bool(ini, SETTINGS, "BridgeDestroy", BridgeDestroy);
	Crates = Spawn_Bool(ini, SETTINGS, "Crates", Crates);
	ShortGame = Spawn_Bool(ini, SETTINGS, "ShortGame", ShortGame);
	BuildOffAlly = Spawn_Bool(ini, SETTINGS, "BuildOffAlly", BuildOffAlly);
	SuperWeapons = Spawn_Bool(ini, SETTINGS, "SuperWeapons", SuperWeapons);
	GameSpeed = ini.Get_Int(SETTINGS, "GameSpeed", GameSpeed);
	MultiEngineer = Spawn_Bool(ini, SETTINGS, "MultiEngineer", MultiEngineer);
	UnitCount = ini.Get_Int(SETTINGS, "UnitCount", UnitCount);
	AIPlayers = ini.Get_Int(SETTINGS, "AIPlayers", AIPlayers);
	AIDifficulty = ini.Get_Int(SETTINGS, "AIDifficulty", AIDifficulty);
	AlliesAllowed = Spawn_Bool(ini, SETTINGS, "AlliesAllowed", AlliesAllowed);
	GameMode = ini.Get_Int(SETTINGS, "GameMode", GameMode);
	HarvesterTruce = Spawn_Bool(ini, SETTINGS, "HarvesterTruce", HarvesterTruce);
	FogOfWar = Spawn_Bool(ini, SETTINGS, "FogOfWar", FogOfWar);
	MCVRedeploy = Spawn_Bool(ini, SETTINGS, "MCVRedeploy", MCVRedeploy);
	AutoDeployMCV = Spawn_Bool(ini, SETTINGS, "AutoDeployMCV", AutoDeployMCV);
	Seed = ini.Get_Int(SETTINGS, "Seed", Seed);
	TechLevel = ini.Get_Int(SETTINGS, "TechLevel", TechLevel);
	Firestorm = Spawn_Bool(ini, SETTINGS, "Firestorm", Firestorm);
	CampaignDifficulty = ini.Get_Int(SETTINGS, "DifficultyModeHuman", CampaignDifficulty);
	CampaignCDifficulty = ini.Get_Int(SETTINGS, "DifficultyModeComputer", CampaignCDifficulty);

	// One key serves twice: the port the game listens on, and this machine's tunnel number.
	TunnelId = ini.Get_Int(SETTINGS, "Port", TunnelId);
	ListenPort = ini.Get_Int(SETTINGS, "Port", ListenPort);
	BindAddress = Read_Text(ini, SETTINGS, "BindAddress", BindAddress);
	TunnelAddress = Read_Text(ini, "Tunnel", "Ip", TunnelAddress);
	TunnelPort = ini.Get_Int("Tunnel", "Port", TunnelPort);

	ConnTimeout = Clamp_Timeout(ini.Get_Int(SETTINGS, "ConnTimeout", ConnTimeout));
	ReconnectTimeout = Clamp_Timeout(ini.Get_Int(SETTINGS, "ReconnectTimeout", ReconnectTimeout));
	AdaptiveTiming = Read_Text(ini, SETTINGS, "TimingPolicy", "") == "Adaptive";

	// A value outside the range the game accepts is passed over, like a key it does not know.
	int const frame_send_rate = ini.Get_Int(SETTINGS, "FrameSendRate", 0);
	int const max_ahead = ini.Get_Int(SETTINGS, "MaxAhead", 0);
	Protocol = ini.Get_Int(SETTINGS, "Protocol", Protocol);
	// HostTiming=yes takes the three keys literally. Without it the client's default values are no choice.
	bool const host_timing = Spawn_Bool(ini, SETTINGS, "HostTiming", false);
	TimingChoice = NetTiming::HostTimingChoice();
	if (frame_send_rate > 0 && NetTiming::Host_Frame_Send_Rate_Is_Valid(static_cast<unsigned int>(frame_send_rate))) {
		TimingChoice.FrameSendRate = static_cast<unsigned int>(frame_send_rate);
	}
	if (max_ahead > 0 && NetTiming::Host_Max_Ahead_Is_Valid(static_cast<unsigned int>(max_ahead))) {
		TimingChoice.MaxAhead = static_cast<unsigned int>(max_ahead);
	}
	// The client writes these keys in every game, with defaults of its own. Those read as no choice unless
	// the file says the host chose (HostTiming=yes), as the lobby built into the game says.
	if (!host_timing && NetTiming::Is_CnCNet_Default_Frame_Send_Rate(TimingChoice.FrameSendRate) && TimingChoice.MaxAhead == 0) {
		TimingChoice.FrameSendRate = 0;
	}
	Protocol = host_timing && Protocol == 0 ? 0 : 2;

	QuickMatch = Spawn_Bool(ini, SETTINGS, "QuickMatch", QuickMatch);
	SkipScoreScreen = Spawn_Bool(ini, SETTINGS, "SkipScoreScreen", SkipScoreScreen);
	AINamesByDifficulty = Spawn_Bool(ini, SETTINGS, "DifficultyBasedAINames", AINamesByDifficulty);
	CoachMode = Spawn_Bool(ini, SETTINGS, "CoachMode", CoachMode);
	AutoSurrender = Spawn_Bool(ini, SETTINGS, "AutoSurrender", AutoSurrender);
	AttackNeutralUnits = Spawn_Bool(ini, SETTINGS, "AttackNeutralUnits", AttackNeutralUnits);
	ScrapMetal = Spawn_Bool(ini, SETTINGS, "ScrapMetal", ScrapMetal);
	PlayMoviesInMultiplayer = Spawn_Bool(ini, SETTINGS, "PlayMoviesInMultiplayer", PlayMoviesInMultiplayer);
	CustomLoadScreen = Read_Text(ini, SETTINGS, "CustomLoadScreen", CustomLoadScreen);
	DifficultyName = Read_Text(ini, SETTINGS, "DifficultyName", DifficultyName);
	SyncLog = Spawn_Bool(ini, SETTINGS, "SyncLog", SyncLog);

	std::string position = Read_Text(ini, SETTINGS, "CustomLoadScreenPos", "");
	if (!position.empty()) {
		int x = 0;
		int y = 0;
		if (std::sscanf(position.c_str(), "%d,%d", &x, &y) == 2) {
			CustomLoadScreenX = x;
			CustomLoadScreenY = y;
		}
	}

	for (int index = 0; index < GLOBAL_FLAG_COUNT; index++) {
		std::string entry = "GlobalFlag" + std::to_string(index);
		GlobalFlags[index] = Spawn_Bool(ini, "GlobalFlags", entry.c_str(), false);
	}

	Read_Slots(ini);
}


namespace {

char const * Yes_No(bool value)
{
	return(value ? "yes" : "no");
}


/// <summary>Writes the packed free color of a seat as the ColorRGB entry Read_Free_Color reads.</summary>
void Write_Free_Color(std::ostringstream & out, unsigned int custom)
{
	if (!Player_Color_Is_Custom(custom)) {
		return;
	}
	RGBClass const rgb = Player_Color_Preview(custom);
	out << "ColorRGB=" << (int)rgb.Get_Red() << "," << (int)rgb.Get_Green() << "," << (int)rgb.Get_Blue() << "\n";
}

}


/// <summary>
/// Writes this reading as launch file text. Only the entries that decide the match are written, in the
/// shape Read_INI and Read_Slots expect: the seat on this machine in [Settings], the other people in
/// [Other1] on, and the computer seats by position in the numbered sections.
/// </summary>
std::string SpawnerConfigClass::Write_INI(void) const
{
	static char const * const _ordinals[SLOT_COUNT] = {
		"HouseAllyOne", "HouseAllyTwo", "HouseAllyThree", "HouseAllyFour",
		"HouseAllyFive", "HouseAllySix", "HouseAllySeven", "HouseAllyEight"
	};

	std::ostringstream out;
	out << "[Settings]\n";
	out << "Scenario=" << ScenarioName << "\n";
	if (!MapName.empty()) {
		out << "UIMapName=" << MapName << "\n";
	}
	out << "IsSinglePlayer=" << Yes_No(IsCampaign) << "\n";
	out << "Bases=" << Yes_No(Bases) << "\n";
	out << "Credits=" << Credits << "\n";
	out << "BridgeDestroy=" << Yes_No(BridgeDestroy) << "\n";
	out << "Crates=" << Yes_No(Crates) << "\n";
	out << "ShortGame=" << Yes_No(ShortGame) << "\n";
	out << "BuildOffAlly=" << Yes_No(BuildOffAlly) << "\n";
	out << "SuperWeapons=" << Yes_No(SuperWeapons) << "\n";
	out << "GameSpeed=" << GameSpeed << "\n";
	out << "MultiEngineer=" << Yes_No(MultiEngineer) << "\n";
	out << "UnitCount=" << UnitCount << "\n";
	out << "AIPlayers=" << AIPlayers << "\n";
	out << "AIDifficulty=" << AIDifficulty << "\n";
	out << "AlliesAllowed=" << Yes_No(AlliesAllowed) << "\n";
	out << "GameMode=" << GameMode << "\n";
	out << "HarvesterTruce=" << Yes_No(HarvesterTruce) << "\n";
	out << "FogOfWar=" << Yes_No(FogOfWar) << "\n";
	out << "MCVRedeploy=" << Yes_No(MCVRedeploy) << "\n";
	out << "AutoDeployMCV=" << Yes_No(AutoDeployMCV) << "\n";
	out << "Seed=" << Seed << "\n";
	out << "TechLevel=" << TechLevel << "\n";
	out << "Firestorm=" << Yes_No(Firestorm) << "\n";
	out << "CoachMode=" << Yes_No(CoachMode) << "\n";
	out << "AutoSurrender=" << Yes_No(AutoSurrender) << "\n";
	out << "AttackNeutralUnits=" << Yes_No(AttackNeutralUnits) << "\n";
	out << "ScrapMetal=" << Yes_No(ScrapMetal) << "\n";
	out << "DifficultyBasedAINames=" << Yes_No(AINamesByDifficulty) << "\n";

	// The local person first: the reader gives [Settings] the first seat, and sorts the people by color.
	int other = 1;
	for (int pass = 0; pass < 2; pass++) {
		for (int index = 0; index < SLOT_COUNT; index++) {
			SlotType const & slot = Slots[index];
			if (slot.Occupancy != OccupancyType::Human || (pass == 0) != (index == LocalSlot)) {
				continue;
			}

			if (pass == 1) {
				out << "\n[Other" << other++ << "]\n";
			}
			out << "Name=" << slot.Name << "\n";
			out << "Side=" << slot.Country << "\n";
			out << "Color=" << slot.Color << "\n";
			Write_Free_Color(out, slot.CustomColor);
			if (slot.IsSpectator) {
				out << "IsSpectator=yes\n";
			}
		}
	}

	out << "\n[HouseCountries]\n";
	for (int index = 0; index < SLOT_COUNT; index++) {
		if (Slots[index].Occupancy == OccupancyType::Computer) {
			out << "Multi" << index + 1 << "=" << Slots[index].Country << "\n";
		}
	}
	out << "\n[HouseColors]\n";
	for (int index = 0; index < SLOT_COUNT; index++) {
		if (Slots[index].Occupancy == OccupancyType::Computer) {
			out << "Multi" << index + 1 << "=" << Slots[index].Color << "\n";
		}
	}
	out << "\n[HouseHandicaps]\n";
	for (int index = 0; index < SLOT_COUNT; index++) {
		if (Slots[index].Occupancy == OccupancyType::Computer) {
			out << "Multi" << index + 1 << "=" << Slots[index].Handicap << "\n";
		}
	}
	out << "\n[SpawnLocations]\n";
	for (int index = 0; index < SLOT_COUNT; index++) {
		if (Slots[index].Occupancy != OccupancyType::Empty) {
			out << "Multi" << index + 1 << "=" << Slots[index].StartingPosition << "\n";
		}
	}

	for (int index = 0; index < SLOT_COUNT; index++) {
		SlotType const & slot = Slots[index];
		int written = 0;
		std::ostringstream allies;
		for (int ally : slot.Alliances) {
			if (ally >= 0 && ally < SLOT_COUNT && written < SLOT_COUNT) {
				allies << _ordinals[written++] << "=" << ally << "\n";
			}
		}
		if (written > 0) {
			out << "\n[Multi" << index + 1 << "_Alliances]\n" << allies.str();
		}
	}
	return(out.str());
}
