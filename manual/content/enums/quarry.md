---
enum_id: QuarryType
slug: quarry
title: Quarry target category
summary: Broad target categories a team attack mission scans for, and that a house records as its preferred target.
representation: integer
bindings:
  key_value_types: []
  scripting_parameter_types: [quarry]
source_files: [code/quarry.hh, code/quarry.cpp, code/team.cpp]
values:
  - { constant: QUARRY_NONE, value: 0, input: "0", meaning: "No category. A team attack line treats it as category 1." }
  - { constant: QUARRY_ANYTHING, value: 1, input: "1", meaning: "Any suitable enemy." }
  - { constant: QUARRY_BUILDINGS, value: 2, input: "2", meaning: "Structures of any kind." }
  - { constant: QUARRY_HARVESTERS, value: 3, input: "3", meaning: "Vehicles and structures whose type sets a nonzero Storage, such as harvesters and refineries." }
  - { constant: QUARRY_INFANTRY, value: 4, input: "4", meaning: "Infantry." }
  - { constant: QUARRY_VEHICLES, value: 5, input: "5", meaning: "Vehicles of any kind, harvesters included. Landed aircraft also count, as do structures that can undeploy into a vehicle, except a construction yard." }
  - { constant: QUARRY_FACTORIES, value: 6, input: "6", meaning: "Structures whose type sets Factory." }
  - { constant: QUARRY_DEFENSE, value: 7, input: "7", meaning: "Armed structures." }
  - { constant: QUARRY_THREAT, value: 8, input: "8", meaning: "The same search as 1. Despite the name, targets near the house's base get no preference." }
  - { constant: QUARRY_POWER, value: 9, input: "9", meaning: "Structures that generate power. A structure with a larger power output is preferred." }
  - { constant: QUARRY_OCCUPIABLE, value: 10, input: "10", meaning: "Structures soldiers can garrison, such as civilian buildings. A structure with a larger MaxNumberOccupants is preferred." }
  - { constant: QUARRY_TECH, value: 11, input: "11", meaning: "Structures with NeedsEngineer=yes, such as the neutral tech buildings." }
---

A quarry is the kind of target a team attacks. The team's [Attack...](/mapping/missions/tmission-attack/) mission names a category, and the team leader searches the whole map for the best enemy of that kind. [Target selection](/systems/target-selection/) explains how the candidates are ranked.

Category 2 accepts every structure, including the factories, armed structures, power plants, garrisonable structures and tech structures that categories 6, 7, 9, 10 and 11 pick out. Each of those limits the search to its own kind of structure.

A team attack line treats a value outside the list as category 1.

[Preferred target...](/mapping/actions/taction-preferred-target/) also takes a quarry category, but it stores the category on a house instead of giving it to a team. That page describes what the stored category does.
