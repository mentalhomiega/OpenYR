---
key: AIIonCannonEngineerValue
summary: The ion cannon rating a computer house gives an enemy engineer.
see_also: [IonCannonDamage, "system:superweapons"]
when_omitted:
  kind: value
  value: ""
---

Applies to an infantry type with [`Engineer=yes`](/keys/engineer/#scope-infantrytype). It is tested before the [vehicle thief rating](/keys/aiioncannonthiefvalue/), so a type that is both takes this one. Infantry that is neither is rated 2.

The computer uses this rating whatever the soldier's strength. A cloaked soldier is rated at random. The computer strikes one of the [highest-rated targets](/systems/superweapons/#the-computers-use), so a higher value puts engineers ahead of more kinds of target.

Each entry belongs to one [difficulty slot](/systems/difficulty/#the-per-difficulty-lists) of the firing house, so give the list three entries. For a computer house the first entry is used at the Hard setting and the last at Easy. The key has no built-in list. If no rules file sets it, the game crashes the first time the computer rates an engineer. A list with fewer than three entries gives an unpredictable rating in the slots it does not cover.
