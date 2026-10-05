/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the INI checker with a small inline catalog: which values it accepts for each form,
// which sections it checks, and what it reports.

#include "inicheck.h"

#include <cstdio>
#include <sstream>
#include <string>

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-76s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


char const CATALOG[] =
	"# test catalog\n"
	"Speed\trules.ini\t*\tAircraftType,BuildingType,InfantryType,UnitType\tinteger\n"
	"Speed\trules.ini\t*\tWeaponType\tspeed\n"
	"Crusher\trules.ini\t*\tUnitType\tboolean\n"
	"Primary\trules.ini\t*\tAircraftType,BuildingType,InfantryType,UnitType\tclass\n"
	"Power\trules.ini\t*\tBuildingType\tinteger\n"
	"AALimit\trules.ini\tAI\tglobal rules\tinteger\n"
	"Gravity\trules.ini\tGeneral\tglobal rules\tinteger\n"
	"Veteran\trules.ini\tGeneral\tglobal rules\tfloating point\n"
	"Easy\trules.ini\t@difficulty\tdifficulty settings\tfloating point\n"
	"Image\tart.ini\t*\tAnimType\tstring\n"
	"TechLevel\tmap file\t@house\tHouse (per-scenario)\tinteger\n"
	"Voxel\tart.ini\t@image\tAircraftType,BuildingType,InfantryType,UnitType\tboolean\n";


IniCheck::Catalog Load_Catalog(void)
{
	IniCheck::Catalog catalog;
	std::istringstream stream(CATALOG);
	std::string error;
	Check(catalog.Load(stream, error), "the inline catalog loads");
	return(catalog);
}


bool Has(IniCheck::Report const & report, IniCheck::FindingType type, char const * key, int line)
{
	for (IniCheck::Finding const & finding : report.Findings) {
		if (finding.Type == type && finding.Key == key && finding.Line == line) {
			return(true);
		}
	}
	return(false);
}


void Test_Values(void)
{
	using IniCheck::ValueKind;
	using IniCheck::Value_Fits;

	Check(Value_Fits(ValueKind::BOOLEAN, "yes") && Value_Fits(ValueKind::BOOLEAN, "No") && Value_Fits(ValueKind::BOOLEAN, "true") && Value_Fits(ValueKind::BOOLEAN, "0"), "a boolean accepts yes, no, true and 0");
	Check(!Value_Fits(ValueKind::BOOLEAN, "maybe") && !Value_Fits(ValueKind::BOOLEAN, "2"), "a boolean refuses maybe and 2");
	Check(Value_Fits(ValueKind::BOOLEAN, "Yikes"), "a boolean reads only the first letter, as the engine does");

	Check(Value_Fits(ValueKind::INTEGER, "-12") && Value_Fits(ValueKind::INTEGER, "+7") && Value_Fits(ValueKind::INTEGER, "5 cells"), "an integer accepts a sign and trailing text");
	Check(Value_Fits(ValueKind::INTEGER, "$1F") && Value_Fits(ValueKind::INTEGER, "1Fh"), "an integer accepts $ and h hexadecimal");
	Check(!Value_Fits(ValueKind::INTEGER, "ten") && !Value_Fits(ValueKind::INTEGER, "$zz") && !Value_Fits(ValueKind::INTEGER, "-"), "an integer refuses ten, $zz and a bare sign");

	Check(Value_Fits(ValueKind::FLOAT, ".5") && Value_Fits(ValueKind::FLOAT, "25%") && Value_Fits(ValueKind::FLOAT, "3"), "a number accepts .5, 25% and 3");
	Check(!Value_Fits(ValueKind::FLOAT, "half"), "a number refuses half");

	Check(Value_Fits(ValueKind::POINT2, "1, -2") && !Value_Fits(ValueKind::POINT2, "1"), "a 2D point needs two whole numbers");
	Check(!Value_Fits(ValueKind::POINT2, "1.5,2"), "a 2D point refuses a fraction before the comma");
	Check(Value_Fits(ValueKind::POINT3, "1.5,2,3") && !Value_Fits(ValueKind::POINT3, "1,2"), "a 3D point needs three numbers");
	Check(Value_Fits(ValueKind::COLOR, "255,128,0") && !Value_Fits(ValueKind::COLOR, "red"), "a color needs three whole numbers");
	Check(Value_Fits(ValueKind::INTEGER_LIST, "1, 2,3") && !Value_Fits(ValueKind::INTEGER_LIST, "1,,3"), "a list of integers refuses an empty item");

	Check(Value_Fits(ValueKind::OTHER, "anything"), "a form the checker cannot test always fits");
	Check(IniCheck::Kind_From_Name("floating point") == ValueKind::FLOAT && IniCheck::Kind_From_Name("speed") == ValueKind::OTHER, "catalog value types map to forms");
}


void Test_Catalog(void)
{
	IniCheck::Catalog catalog;
	std::istringstream bad("Speed\trules.ini\t*\n");
	std::string error;
	Check(!catalog.Load(bad, error) && !error.empty(), "a catalog line without five fields is refused");

	IniCheck::Catalog good = Load_Catalog();
	Check(good.Find("Speed") != nullptr && good.Find("Speed")->size() == 2, "a key keeps every scope");
	Check(good.Find("speed") == nullptr && good.Find_Other_Case("speed") == "Speed", "lookups are case sensitive, with a case-blind hint");
	Check(good.Has_Section("rules.ini", "General") && !good.Has_Section("rules.ini", "Easy"), "only literal sections count as known sections");
}


void Test_Rules(void)
{
	IniCheck::Catalog catalog = Load_Catalog();
	char const text[] =
		"\xEF\xBB\xBF[General]\r\n"         // 1
		"Gravity=6\r\n"                     // 2
		"Speeed=5 ; misspelled\r\n"         // 3
		"AALimit=10\r\n"                    // 4
		"Veteran=fast\r\n"                  // 5
		"\r\n"                              // 6
		"[VehicleTypes]\r\n"                // 7
		"0=MYTANK\r\n"                      // 8
		"[BuildingTypes]\r\n"               // 9
		"0=MYPLANT\r\n"                     // 10
		"[MYTANK]\r\n"                      // 11
		"Speed=6\r\n"                       // 12
		"speed=6\r\n"                       // 13
		"Crusher=maybe\r\n"                 // 14
		"Power=100\r\n"                     // 15
		"Primary=MYGUN\r\n"                 // 16
		"=orphan\r\n"                       // 17
		"Empty=\r\n"                        // 18
		"[MYPLANT]\r\n"                     // 19
		"Power=100\r\n"                     // 20
		"Crusher=yes\r\n"                   // 21
		"[MYGUN]\r\n"                       // 22
		"Damage=10\r\n";                    // 23

	IniCheck::Report const report = IniCheck::Check_Rules(catalog, text);

	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Speeed", 3), "a misspelled key is reported with its line");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "AALimit", 4), "a key read only in another section is reported");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Veteran", 5), "a value the engine cannot read is reported");
	Check(Has(report, IniCheck::FindingType::WRONG_CASE, "speed", 13), "a key in the wrong case is reported as such");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Crusher", 14), "an object section's values are checked by the type's form");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Power", 15), "a key of another type kind is reported");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Crusher", 21), "a vehicle key in a structure section is reported");
	Check(report.Findings.size() == 7, "nothing else is reported, including lines the engine skips");
	Check(report.UncheckedSections.size() == 1 && report.UncheckedSections[0] == "MYGUN", "a section no list names is left unchecked");

	bool ordered = true;
	for (std::size_t index = 1; index < report.Findings.size(); index++) {
		if (report.Findings[index - 1].Line > report.Findings[index].Line) {
			ordered = false;
		}
	}
	Check(ordered, "findings come in line order");

	IniCheck::Report const art = IniCheck::Check_Rules(catalog, text, "art.ini");
	Check(Has(art, IniCheck::FindingType::UNKNOWN_KEY, "Speed", 12) && !Has(art, IniCheck::FindingType::UNKNOWN_KEY, "Gravity", 2), "only the named file's scopes are used");

	Check(IniCheck::Format(report.Findings.front()) == "line 3: [General] Speeed=5: the engine does not read this key in this section", "a finding formats as one line");
}


void Test_Art(void)
{
	IniCheck::Catalog const catalog = Load_Catalog();
	char const rules[] =
		"[VehicleTypes]\n"
		"1=MTNK\n"
		"2=HTNK\n"
		"[HTNK]\n"
		"Image=APOC\n";
	char const art[] =
		"[MTNK]\n"
		"Voxel=yes\n"
		"Bogus=1\n"
		"[APOC]\n"
		"Voxel=maybe\n"
		"[HTNK]\n"
		"Voxel=yes\n";
	IniCheck::Report const report = IniCheck::Check_Art(catalog, art, rules);
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Bogus", 3) && !Has(report, IniCheck::FindingType::BAD_VALUE, "Voxel", 2), "a type's art section is checked under its own name");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Voxel", 5), "a type with Image= is checked under the image's section");
	Check(report.UncheckedSections.size() == 1 && report.UncheckedSections.front() == "HTNK", "a type's own name is not its art section when Image= names another");
}


void Test_Map_Listing(void)
{
	IniCheck::Catalog const catalog = Load_Catalog();
	char const map[] =
		"[Houses]\n"
		"0=Americans\n"
		"[Americans]\n"
		"TechLevel=8\n"
		"Bogus=1\n"
		"[MTNK]\n"
		"Crusher=maybe\n";
	IniCheck::Report const report = IniCheck::Check_Map(catalog, map, "[VehicleTypes]\n1=MTNK\n");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Crusher", 7), "a map override of a type the rules list is checked");
	Check(!Has(report, IniCheck::FindingType::UNKNOWN_KEY, "TechLevel", 4) && Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Bogus", 5), "a house the map lists is checked against the map's house keys");
	IniCheck::Report const alone = IniCheck::Check_Map(catalog, map, "");
	Check(!Has(alone, IniCheck::FindingType::BAD_VALUE, "Crusher", 7), "without the rules' lists the map's type section is not placed");
}

}


int main(void)
{
	Test_Values();
	Test_Catalog();
	Test_Rules();
	Test_Art();
	Test_Map_Listing();

	std::printf("\n%d failure(s)\n", Failures);
	return(Failures == 0 ? 0 : 1);
}
