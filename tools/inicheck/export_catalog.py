#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2026 OpenTS contributors
"""Writes the INI checker's key catalog from manual/data/ini-keys.yaml.

Each output line holds one scope as tab separated fields: key, file, section,
applies-to list and value type. The section is the literal section name, "*"
for an object section, or "@" and the section source for a section the checker
cannot place. A key holding a range such as "Weapon{1-18}" stands for every key
the engine builds from a number in that range. A lower bound with a leading zero, as in
"Tile{01-}Anim", stands for a number printed with at least that many digits.
"""

import argparse
import re
import sys
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]

# The object kinds whose rules sections read the techno type keys.
TECHNO_KINDS = ["AircraftType", "BuildingType", "InfantryType", "UnitType"]

# Keys the engine builds from a number or a name that the manual's extractor does not list. Each
# range starts where the reading loop starts; the loops end at NumberOfDocks, Upgrades,
# TilesInSet and the custom foundation's size, which have no fixed limit.
PATTERN_SCOPES = [
    # BuildingTypeClass::Read_INI.
    ("DockingOffset{0-}", "art.ini", "@image", ["BuildingType"], "point (x,y,z)"),
    ("PowerUp{1-}Anim", "art.ini", "@image", ["BuildingType"], "string"),
    ("PowerUp{1-}DamagedAnim", "art.ini", "@image", ["BuildingType"], "string"),
    ("PowerUp{1-}LocXX", "art.ini", "@image", ["BuildingType"], "integer"),
    ("PowerUp{1-}LocYY", "art.ini", "@image", ["BuildingType"], "integer"),
    ("PowerUp{1-}LocZZ", "art.ini", "@image", ["BuildingType"], "integer"),
    ("PowerUp{1-}YSort", "art.ini", "@image", ["BuildingType"], "integer"),
    # BuildingTypeClass::Read_INI: the burning damage fires, and the cells a structure adds to and
    # removes from the cells it occupies. Each table holds eight entries.
    ("DamageFireOffset{0-7}", "art.ini", "@image", ["BuildingType"], "point (x,y)"),
    ("AddOccupy{1-8}", "art.ini", "@image", ["BuildingType"], "point (x,y)"),
    ("RemoveOccupy{1-8}", "art.ini", "@image", ["BuildingType"], "point (x,y)"),
    # BuildingTypeClass::Read_Custom_Foundation: the cells of a Foundation=Custom building and of
    # its outline, numbered from 0 up to Foundation.X * Foundation.Y and FoundationOutline.Length.
    ("Foundation.{0-}", "art.ini", "@image", ["BuildingType"], "point (x,y)"),
    ("FoundationOutline.{0-}", "art.ini", "@image", ["BuildingType"], "point (x,y)"),
    # TechnoTypeClass::Read_Insignia_INI: a key for each rank.
    ("Insignia.Rookie", "rules.ini", "*", TECHNO_KINDS, "string"),
    ("Insignia.Veteran", "rules.ini", "*", TECHNO_KINDS, "string"),
    ("Insignia.Elite", "rules.ini", "*", TECHNO_KINDS, "string"),
    ("InsigniaFrame.Rookie", "rules.ini", "*", TECHNO_KINDS, "integer"),
    ("InsigniaFrame.Veteran", "rules.ini", "*", TECHNO_KINDS, "integer"),
    ("InsigniaFrame.Elite", "rules.ini", "*", TECHNO_KINDS, "integer"),
    # WarheadTypeClass::Read_INI: a percentage for each armor type and, for each, three switches.
    # The star stands for the armor's name, which the rules' [ArmorTypes] declare.
    ("Versus.{*}", "rules.ini", "*", ["WarheadType"], "string"),
    ("Versus.{*}.ForceFire", "rules.ini", "*", ["WarheadType"], "boolean"),
    ("Versus.{*}.Retaliate", "rules.ini", "*", ["WarheadType"], "boolean"),
    ("Versus.{*}.PassiveAcquire", "rules.ini", "*", ["WarheadType"], "boolean"),
    # IsometricTileTypeClass::Init: the per-tile keys, numbered from 1 as "%02d" up to the set's
    # TilesInSet. They are read from the section the set's SetName= names, not from the TileSet
    # section itself. A lower bound written with a leading zero stands for that printed width.
    ("Tile{01-}Anim", "theater control file", "@tile-set-name", ["tile controls"], "string"),
    ("Tile{01-}XOffset", "theater control file", "@tile-set-name", ["tile controls"], "integer"),
    ("Tile{01-}YOffset", "theater control file", "@tile-set-name", ["tile controls"], "integer"),
    ("Tile{01-}AttachesTo", "theater control file", "@tile-set-name", ["tile controls"], "integer"),
    ("Tile{01-}ZAdjust", "theater control file", "@tile-set-name", ["tile controls"], "integer"),
]


# RulesClass::RocketTypeStruct::Read reads one missile's flight settings from the keys named after a
# prefix: V3Rocket, DMisl and CMisl in [General], and Missile. in the rules section of an aircraft type.
ROCKET_KEYS = [
    ("PauseFrames", "integer"),
    ("TiltFrames", "integer"),
    ("PitchInitial", "floating point"),
    ("PitchFinal", "floating point"),
    ("TurnRate", "floating point"),
    ("RaiseRate", "integer"),
    ("Acceleration", "floating point"),
    ("Altitude", "integer"),
    ("Damage", "integer"),
    ("EliteDamage", "integer"),
    ("BodyLength", "integer"),
    ("LazyCurve", "boolean"),
    ("Type", "class"),
]
for _prefix in ("V3Rocket", "DMisl", "CMisl"):
    for _name, _type in ROCKET_KEYS:
        PATTERN_SCOPES.append((_prefix + _name, "rules.ini", "General", ["global rules"], _type))
for _name, _type in ROCKET_KEYS:
    PATTERN_SCOPES.append(("Missile." + _name, "rules.ini", "*", ["AircraftType"], _type))


# InfantryTypeClass::Read_Sequence_INI reads one do-string from the section an infantry art
# section names with Sequence=, under each name in this table.
SEQUENCE_SOURCE = ROOT / "code/infatype.cpp"


def sequence_names(source):
    """The do-string key names listed in the SequenceName table of infatype.cpp."""
    text = source.read_text(encoding="utf-8")
    table = re.search(r"SequenceName\s*\[[^\]]*\]\s*=\s*\{(.*?)\}\s*;", text, re.S)
    names = re.findall(r'"([^"]+)"', table.group(1)) if table else []
    if not names:
        sys.exit(f"{source}: no SequenceName table found")
    return names


# Sections the engine names in code that the manual's extractor describes by source only.
# RulesClass::Difficulty reads the same keys from each of these three sections of rules.ini.
SOURCE_SECTIONS = {
    "difficulty": ["Easy", "Normal", "Difficult"],
}


def section_fields(section):
    """The section field of each catalog line that one manual scope writes."""
    if section.get("kind") == "identifier" and section.get("source") in SOURCE_SECTIONS:
        return SOURCE_SECTIONS[section["source"]]
    return [section_field(section)]


def section_field(section):
    kind = section.get("kind")
    if kind == "literal":
        return section["name"]
    if kind == "identifier" and section.get("source") == "object-type":
        return "*"
    return "@" + str(section.get("source") or kind)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--keys", type=Path, default=ROOT / "manual/data/ini-keys.yaml")
    parser.add_argument("--sequence-source", type=Path, default=SEQUENCE_SOURCE, help="source file holding the SequenceName table")
    parser.add_argument("--output", type=Path, help="file to write; standard output when omitted")
    args = parser.parse_args()

    with args.keys.open(encoding="utf-8") as stream:
        catalog = yaml.safe_load(stream)

    lines = ["# Generated by tools/inicheck/export_catalog.py; do not edit."]
    for name in sorted(catalog):
        for scope in catalog[name]["scopes"]:
            for section in section_fields(scope["section"]):
                fields = [
                    catalog[name]["key"],
                    scope["file"],
                    section,
                    ",".join(scope.get("applies_to") or []),
                    scope.get("value_type") or "",
                ]
                if any("\t" in field or "\n" in field for field in fields):
                    sys.exit(f"{name}: a field holds a tab or line break")
                lines.append("\t".join(fields))
    for key, file, section, applies_to, value_type in PATTERN_SCOPES:
        lines.append("\t".join([key, file, section, ",".join(applies_to), value_type]))
    for key in sequence_names(args.sequence_source):
        lines.append("\t".join([key, "art.ini", "@sequence", "InfantryType", "infantry sequence"]))

    text = "\n".join(lines) + "\n"
    if args.output:
        args.output.write_text(text, encoding="utf-8", newline="\n")
    else:
        sys.stdout.write(text)


if __name__ == "__main__":
    main()
