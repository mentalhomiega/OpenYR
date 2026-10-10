---
key: Cost
scope: housetype
label: Country price multiplier
no_effect: true
see_also: ["system:difficulty"]
when_omitted:
  kind: value
  value: "1.0"
---

A house of this country combines this multiplier with [the difficulty section's `Cost=`](/keys/cost/#scope-difficulty-settings) [when it is given its difficulty slot](/systems/difficulty/#how-the-figures-are-combined). A campaign game leaves the country's value out.

Nothing in the game reads the combined figure, so no value here changes a price. The country's price multipliers are [`CostInfantryMult`](/keys/costinfantrymult/), [`CostUnitsMult`](/keys/costunitsmult/), [`CostAircraftMult`](/keys/costaircraftmult/), [`CostBuildingsMult`](/keys/costbuildingsmult/) and [`CostDefensesMult`](/keys/costdefensesmult/); [What a house pays](/keys/cost/#what-a-house-pays) shows how they apply.
