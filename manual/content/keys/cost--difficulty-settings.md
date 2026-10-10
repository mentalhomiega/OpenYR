---
key: Cost
scope: difficulty-settings
label: Difficulty price multiplier
no_effect: true
see_also: ["system:difficulty"]
when_omitted:
  kind: value
  value: "1"
  note: The difficulty block is re-read from fixed defaults whenever its section is present, so a later file that contains the section without this key restores 1 rather than keeping the earlier value.
---

`[Easy]`, `[Normal]` and `[Difficult]` each set their own multiplier. A house combines the one for [its difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot) with, outside a campaign game, [its country's `Cost=`](/keys/cost/#scope-housetype).

Nothing in the game reads the combined figure, so no value here changes a price. A house pays each object's [`Cost=`](/keys/cost/#scope-aircrafttype) times the factors in [What a house pays](/keys/cost/#what-a-house-pays), and a factory charges that price over the production steps.
