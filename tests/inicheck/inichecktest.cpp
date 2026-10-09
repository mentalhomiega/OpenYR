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
	"Damage\trules.ini\t*\tWeaponType\tinteger\n"
	"Projectile\trules.ini\t*\tWeaponType\tclass\n"
	"Warhead\trules.ini\t*\tWeaponType\tclass\n"
	"Arcing\trules.ini\t*\tBulletType\tboolean\n"
	"Verses\trules.ini\t*\tWarheadType\tstring\n"
	"Weapon{1-18}\trules.ini\t*\tAircraftType,BuildingType,InfantryType,UnitType\tclass\n"
	"BurstDelay{0-3}\trules.ini\t*\tWeaponType\tinteger\n"
	"DockingOffset{0-}\tart.ini\t@image\tBuildingType\tpoint (x,y,z)\n"
	"FirePower\trules.ini\tEasy\tdifficulty settings\tfloating point\n"
	"FirePower\trules.ini\tDifficult\tdifficulty settings\tfloating point\n"
	"BuildSlowdown\trules.ini\tEasy\tdifficulty settings\tboolean\n"
	"BuildSlowdown\trules.ini\tDifficult\tdifficulty settings\tboolean\n"
	"Voxel\tart.ini\t@image\tAircraftType,BuildingType,InfantryType,UnitType\tboolean\n"
	"Trailer\tart.ini\t@image\tBulletType\tclass\n";


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
	Check(good.Has_Section("rules.ini", "General") && !good.Has_Section("rules.ini", "@difficulty"), "only literal sections count as known sections");
	Check(good.Find("Weapon{1-18}") == nullptr && good.Count() == 22, "a key holding a range is kept as a pattern");
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

	Check(IniCheck::KeyPattern::Parse("Tile{01-}Anim", pattern) && pattern.Width == 2 && pattern.Low == 1, "a lower bound with a leading zero sets the printed width");
	Check(pattern.Matches("Tile01Anim") && pattern.Matches("Tile09Anim") && pattern.Matches("Tile12Anim") && pattern.Matches("Tile100Anim"), "a padded pattern matches the numbers as %02d prints them");
	Check(!pattern.Matches("Tile1Anim") && !pattern.Matches("Tile00Anim") && !pattern.Matches("Tile012Anim") && !pattern.Matches("TileAnim"), "a padded pattern refuses an unpadded number, zero, extra zeros and no number");
	Check(IniCheck::KeyPattern::Parse("Territory{00-}", pattern) && pattern.Matches("Territory00") && pattern.Matches("Territory07") && !pattern.Matches("Territory0"), "a padded pattern may start at zero");
	Check(IniCheck::KeyPattern::Parse("Slot{01-12}", pattern) && pattern.Matches("Slot12") && !pattern.Matches("Slot13"), "a padded pattern keeps its upper bound");

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


void Test_Difficulty(void)
{
	IniCheck::Catalog catalog = Load_Catalog();
	char const text[] =
		"[Easy]\n"                          // 1
		"FirePower=1.1\n"                   // 2
		"BuildSlowdown=maybe\n"             // 3
		"Cost=1\n"                          // 4
		"[Difficult]\n"                     // 5
		"FirePower=big\n"                   // 6
		"BuildSlowdown=no\n"                // 7
		"[VehicleTypes]\n"                  // 8
		"0=MYTANK\n"                        // 9
		"[MYTANK]\n"                        // 10
		"FirePower=1\n"                     // 11
		"[Hard]\n"                          // 12
		"FirePower=1\n";                    // 13

	IniCheck::Report const report = IniCheck::Check_Rules(catalog, text);

	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "BuildSlowdown", 3) && Has(report, IniCheck::FindingType::BAD_VALUE, "FirePower", 6), "a difficulty section's values are checked by its keys' forms");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Cost", 4), "a key the catalog does not give a difficulty section is reported");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "FirePower", 11), "a difficulty key in a vehicle section is reported");
	Check(report.Findings.size() == 4, "nothing else is reported");
	Check(report.UncheckedSections.size() == 1 && report.UncheckedSections[0] == "Hard", "a section the engine does not read as a difficulty is left unchecked");
}


void Test_Art_Placement(void)
{
	IniCheck::Catalog catalog = Load_Catalog();
	char const rules[] =
		"[VehicleTypes]\n"                  // 1
		"0=MYTANK\n"                        // 2
		"1=MYTRUCK\n"                       // 3
		"[BuildingTypes]\n"                 // 4
		"0=MYPLANT\n"                       // 5
		"[Animations]\n"                    // 6
		"0=MYFLASH\n"                       // 7
		"[MYTANK]\n"                        // 8
		"Primary=MYGUN\n"                   // 9
		"[MYTRUCK]\n"                       // 10
		"Image=MYTANK\n"                    // 11
		"[MYPLANT]\n"                       // 12
		"Image=MYSKIN\n"                    // 13
		"[MYGUN]\n"                         // 14
		"Projectile=MYSHELL\n"              // 15
		"[MYSHELL]\n"                       // 16
		"Image=MYSHELLART\n"                // 17
		"Arcing=yes\n"                      // 18
		"[MYWARHEAD]\n"                     // 19
		"Verses=100%\n";                    // 20

	char const art[] =
		"[MYTANK]\n"                        // 1
		"Voxel=yes\n"                       // 2
		"Voxel=maybe\n"                     // 3
		"DockingOffset0=1,2,3\n"            // 4
		"[MYTRUCK]\n"                       // 5
		"Voxel=yes\n"                       // 6
		"[MYPLANT]\n"                       // 7
		"Voxel=yes\n"                       // 8
		"[MYSKIN]\n"                        // 9
		"DockingOffset1=1,2\n"              // 10
		"Voxel=yes\n"                       // 11
		"[MYFLASH]\n"                       // 12
		"Image=FLASHART\n"                  // 13
		"Voxel=yes\n"                       // 14
		"[MYSHELLART]\n"                    // 15
		"Trailer=MYTRAIL\n"                 // 16
		"Voxel=yes\n"                       // 17
		"[MYSHELL]\n"                       // 18
		"Trailer=MYTRAIL\n"                 // 19
		"[MYGUN]\n"                         // 20
		"Voxel=yes\n";                      // 21

	IniCheck::Report const report = IniCheck::Check_Art(catalog, art, rules);

	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Voxel", 3), "an object's art section takes the art key forms");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "DockingOffset0", 4), "a structure art key in a vehicle's art section is reported");
	Check(!Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Voxel", 2) && !Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Voxel", 11), "a section is checked as the kinds that read it");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "DockingOffset1", 10), "the section an Image= names is checked as that structure's art");
	Check(Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Voxel", 14) && !Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Image", 13), "an animation reads the section of its own name");
	Check(!Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Trailer", 16) && Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Voxel", 17), "a projectile with an Image= reads that section as a projectile");
	Check(report.UncheckedSections.size() == 4 && report.UncheckedSections[0] == "MYTRUCK" && report.UncheckedSections[1] == "MYPLANT" && report.UncheckedSections[2] == "MYSHELL" && report.UncheckedSections[3] == "MYGUN",
		"a type's own name is unchecked when its Image= names another section, and a weapon has no art section");
	Check(report.Findings.size() == 5, "nothing else is reported");
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


void Test_Rules_Overlay(void)
{
	IniCheck::Catalog const catalog = Load_Catalog();
	char const overlay[] =
		"[MTNK]\n"
		"Crusher=maybe\n"
		"[InfantryTypes]\n"
		"900=NEWGI\n"
		"[NEWGI]\n"
		"Speed=fast\n"
		"Bogus=1\n";
	IniCheck::Report const report = IniCheck::Check_Rules_Overlay(catalog, overlay, "[VehicleTypes]\n1=MTNK\n");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Crusher", 2), "an overlay's section for a type the rules list is checked");
	Check(Has(report, IniCheck::FindingType::BAD_VALUE, "Speed", 6) && Has(report, IniCheck::FindingType::UNKNOWN_KEY, "Bogus", 7), "a type the overlay lists itself is checked");
	IniCheck::Report const alone = IniCheck::Check_Rules(catalog, overlay);
	Check(!Has(alone, IniCheck::FindingType::BAD_VALUE, "Crusher", 2), "checked as a rules file of its own, the overlay's type section is not placed");
}

}


int main(void)
{
	Test_Values();
	Test_Catalog();
	Test_Patterns();
	Test_Rules();
	Test_Art();
	Test_Map_Listing();
	Test_Rules_Overlay();
	Test_References();
	Test_Difficulty();
	Test_Art_Placement();

	std::printf("\n%d failure(s)\n", Failures);
	return(Failures == 0 ? 0 : 1);
}
