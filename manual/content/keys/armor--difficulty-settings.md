---
key: Armor
scope: difficulty-settings
label: Difficulty armor figure
no_effect: true
see_also: ["system:difficulty"]
when_omitted:
  kind: value
  value: "1"
  note: The difficulty block is re-read from fixed defaults whenever its section is present, so a later file that contains the section but not this key resets the figure to 1.
---

A house combines the figure for [its difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot) with, outside a campaign game, [its country's `Armor=`](/keys/armor/#scope-housetype). Nothing in the game reads the combined figure, so no value here changes the damage anything takes. The divisors that do are the country's [`ArmorInfantryMult`](/keys/armorinfantrymult/) family.
