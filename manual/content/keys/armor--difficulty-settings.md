---
key: Armor
scope: difficulty-settings
label: Difficulty armor figure
see_also: ["system:difficulty"]
when_omitted:
  kind: value
  value: "1"
  note: The difficulty block is re-read from fixed defaults whenever its section is present, so a later file that contains the section but not this key resets the figure to 1.
---

The figure is read from the difficulty section but does not change damage. A house's armor divisor is its country's [`Armor=`](/keys/armor/#scope-housetype) in every game, so the difficulty slot leaves the damage its objects take unchanged.
