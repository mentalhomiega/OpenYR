---
key: AIIonCannonTechCenterValue
summary: The ion cannon rating a computer house gives an enemy structure listed in BuildTech.
see_also: [BuildTech, IonCannonDamage, "system:superweapons"]
when_omitted:
  kind: value
  value: ""
---

Applies to a structure listed in [`BuildTech`](/keys/buildtech/). It is the last of the structure tests, so a tech center that matches an [earlier test](/systems/superweapons/#the-computers-use), such as a positive [`Power=`](/keys/power/#scope-buildingtype) or [`IsBaseDefense=yes`](/keys/isbasedefense/#scope-buildingtype), takes that rating instead. A structure that matches none of the tests is rated 4.

We use this rating whatever the structure's strength. A cloaked structure is rated at random. The computer strikes one of the [highest-rated targets](/systems/superweapons/#the-computers-use), so a higher value puts tech centers ahead of more kinds of target.

Each entry belongs to one [difficulty slot](/systems/difficulty/#the-per-difficulty-lists) of the firing house, so give the list three entries. For a computer house the first entry is used at the Hard setting and the last at Easy. The key has no built-in list. If no rules file sets it, the game crashes the first time the computer rates a tech center. A list with fewer than three entries gives an unpredictable rating in the slots it does not cover.
