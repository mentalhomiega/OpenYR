---
enum_id: ArmorType
slug: armor
title: Armor
summary: Armor classes used when warheads calculate their effectiveness against a target.
representation: token
bindings:
  key_value_types: [ArmorType]
  scripting_parameter_types: []
source_files: [code/armor.hh, code/const.cpp, code/weapon.cpp]
values:
  - { constant: ARMOR_NONE, value: 0, input: "none", meaning: "No armor." }
  - { constant: ARMOR_FLAK, value: 1, input: "flak", meaning: "Flak armor." }
  - { constant: ARMOR_PLATE, value: 2, input: "plate", meaning: "Plate armor." }
  - { constant: ARMOR_LIGHT, value: 3, input: "light", meaning: "Light armor." }
  - { constant: ARMOR_MEDIUM, value: 4, input: "medium", meaning: "Medium armor." }
  - { constant: ARMOR_HEAVY, value: 5, input: "heavy", meaning: "Heavy armor." }
  - { constant: ARMOR_WOOD, value: 6, input: "wood", meaning: "Wood; also tested by wall and tree rules." }
  - { constant: ARMOR_STEEL, value: 7, input: "steel", meaning: "Steel armor." }
  - { constant: ARMOR_CONCRETE, value: 8, input: "concrete", meaning: "Concrete armor." }
  - { constant: ARMOR_SPECIAL_1, value: 9, input: "special_1", meaning: "First special armor class." }
  - { constant: ARMOR_SPECIAL_2, value: 10, input: "special_2", meaning: "Second special armor class." }
---

Armor names are matched without regard to case. A mod can add further classes with [`[ArmorTypes]`](/systems/armor-types/). How much damage a warhead deals to each class is set by that warhead's [`Verses`](/keys/verses/) list, which has one entry per class in the order shown above. A warhead's `Versus.<armor>` entries set the same figure by name, for these classes and for declared ones.

The `wood` class is also tested outside `Verses`. A [`Wood=yes`](/keys/wood/) warhead can reduce a wall overlay whose class is `wood`. [`Armor`](/keys/armor/#scope-aircrafttype) covers both.

:::caution[Check the spelling of armor names]
A misspelled armor name is not rejected. It reads as `none`, an ordinary class, so every warhead scales its damage to the type by the first entry of that warhead's `Verses` list.
:::
