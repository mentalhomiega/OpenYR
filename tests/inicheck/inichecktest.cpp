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
	"Damage\trules.ini\t*\tWeaponType\tinteger\n"
	"Projectile\trules.ini\t*\tWeaponType\tclass\n"
	"Warhead\trules.ini\t*\tWeaponType\tclass\n"
	"Arcing\trules.ini\t*\tBulletType\tboolean\n"
	"Verses\trules.ini\t*\tWarheadType\tstring\n"
	"Weapon{1-18}\trules.ini\t*\tAircraftType,BuildingType,InfantryType,UnitType\tclass\n"
	"BurstDelay{0-3}\trules.ini\t*\tWeaponType\tinteger\n"
	"DockingOffset{0-}\tart.ini\t@image\tBuildingType\tpoint (x,y,z)\n";


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
	Check(good.Find("Weapon{1-18}") == nullptr && good.Count() == 17, "a key holding a range is kept as a pattern");
}


void Test_Patterns(void)
{
	IniCheck::KeyPattern pattern;
	Check(IniCheck::KeyPattern::Parse("Weapon{1-18}FLH", pattern) && pattern.Prefix == "Weapon" && pattern.Suffix == "FLH", "a pattern splits around its range");
	Check(pattern.Matches("Weapon1FLH") && pattern.Matches("Weapon18FLH"), "a pattern matches both ends of its range");
	Check(!pattern.Matches("Weapon0FLH") && !pattern.Matches("Weapon19FLH"), "a pattern refuses numbers outside its range");
	Check(!pattern.Matches("Weapon01FLH") && !pattern.Matches("WeaponFLH") && !pattern.Matches("Weapon1xFLH"), "a pattern refuses a leading zero, no number and other text");

	Check(IniCheck::KeyPattern::Parse("DockingOffset{0-}", pattern) && pattern.Matches("DockingOffset0") && pattern.Matches("DockingOffset250"), "a pattern with an open end takes any larger number");
	Check(!IniCheck::KeyPattern::Parse("Weapon{1}", pattern) && !IniCheck::KeyPattern::Parse("Weapon{5-2}", pattern) && !IniCheck::KeyPattern::Parse("Weapon", pattern), "a malformed range is refused");

	IniCheck::Catalog catalog = Load_Catalog();
	Check(catalog.Find_All("Weapon3").size() == 1 && catalog.Find_All("Weapon19").empty(), "the catalog finds a key through its pattern");
	Check(catalog.Find_All("DockingOffset7").size() == 1 && catalog.Find_All("DockingOffset7")[0]->File == "art.ini", "an art pattern keeps its scope");
}


void Test_References(void)
{
	IniCheck::Catalog catalog = Load_Catalog();
	char const text[] =
		"[VehicleTypes]\n"                   // 1
		"0=MYTANK\n"                         // 2
		"[MYTANK]\n"                         // 3
		"Primary=MYGUN\n"                    // 4
		"Weapon2=OTHERGUN\n"                 // 5
		"Weapon01=ZEROGUN\n"                 // 6
		"Projectile=NOTREAD\n"               // 7
		"[MYGUN]\n"                          // 8
		"Damage=ten\n"                       // 9
		"Projectile=MYSHELL\n"               // 10
		"Warhead=MYWARHEAD\n"                // 11
		"BurstDelay3=5\n"                    // 12
		"BurstDelay4=5\n"                    // 13
		"[OTHERGUN]\n"                       // 14
		"Speeed=1\n"                         // 15
		"[MYSHELL]\n"                        // 16
		"Arcing=maybe\n"                     // 17
		"[MYWARHEAD]\n"                      // 18
		"Verses=100%\n"                      // 19
		"Damage=5\n"                         // 20
		"[ZEROGUN]\n"                        // 21
		"Damage=1\n"                         // 22
		"[NOTREAD]\n"                        // 23
		"Arcing=yes\n";                      // 24

	IniCheck::Report const report = IniCheck::Check_Rules(catalog, text);

	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Damage", 9), "a weapon named by Primary= is checked as a weapon");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Speeed", 15), "a weapon named by a numbered Weapon key is checked");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Arcing", 17), "a projectile named by the weapon's Projectile= is checked");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Damage", 20), "a warhead named by the weapon's Warhead= is checked as a warhead");
	Check(!Has(report, IniCheck::FindingType::UNKNOWN_KEY, "BurstDelay3", 12) && Has(report, IniCheck::FindingType::UNKNOWN_KEY, "BurstDelay4", 13), "a numbered key is read only inside its range");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Weapon01", 6) && Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Projectile", 7), "keys the type does not read are reported");
	Check(report.Findings.size() == 7, "nothing else is reported");
	Check(report.UncheckedSections.size() == 2 && report.UncheckedSections[0] == "ZEROGUN" && report.UncheckedSections[1] == "NOTREAD", "a reference from a key the type does not read is not followed");
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
		"Damage=10\r\n"                     // 23
		"[ORPHAN]\r\n"                      // 24
		"Damage=10\r\n";                    // 25

	IniCheck::Report const report = IniCheck::Check_Rules(catalog, text);

	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Speeed", 3), "a misspelled key is reported with its line");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "AALimit", 4), "a key read only in another section is reported");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Veteran", 5), "a value the engine cannot read is reported");
	Check(Has(report, IniCheck::FindingType::WRONG_CASE, "speed", 13), "a key in the wrong case is reported as such");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Crusher", 14), "an object section's values are checked by the type's form");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Power", 15), "a key of another type kind is reported");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Crusher", 21), "a vehicle key in a structure section is reported");
	Check(report.Findings.size() == 7, "nothing else is reported, including lines the engine skips");
	Check(report.UncheckedSections.size() == 1 && report.UncheckedSections[0] == "ORPHAN", "a section no list or reference names is left unchecked");

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

}


int main(void)
{
	Test_Values();
	Test_Catalog();
	Test_Patterns();
	Test_Rules();
	Test_References();

	std::printf("\n%d failure(s)\n", Failures);
	return(Failures == 0 ? 0 : 1);
}
