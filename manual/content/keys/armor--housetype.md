---
key: Armor
scope: housetype
label: Armor bias
no_effect: true
see_also: ["system:difficulty", ArmorInfantryMult, ArmorUnitsMult, ArmorAircraftMult, ArmorBuildingsMult, ArmorDefensesMult]
when_omitted:
  kind: value
  value: "1.0"
---

A house of this country combines this value with [the difficulty section's `Armor=`](/keys/armor/#scope-difficulty-settings) [when it is given its difficulty slot](/systems/difficulty/#how-the-figures-are-combined), outside a campaign game. Nothing in the game reads the combined figure, so no value here changes the damage anything takes.

The damage a country's objects take is divided by the multiplier for the kind of object: [`ArmorInfantryMult`](/keys/armorinfantrymult/), [`ArmorUnitsMult`](/keys/armorunitsmult/), [`ArmorAircraftMult`](/keys/armoraircraftmult/), [`ArmorBuildingsMult`](/keys/armorbuildingsmult/) or [`ArmorDefensesMult`](/keys/armordefensesmult/).
