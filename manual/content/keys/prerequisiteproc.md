---
key: PrerequisiteProc
summary: The BuildingTypes that satisfy a PROC prerequisite.
see_also: [PrerequisiteProcAlternate, "system:production"]
when_omitted:
  kind: value
  value: ""
---

A house satisfies a `PROC` entry in a [`Prerequisite=`](/keys/prerequisite/) list by owning at least one structure of any type on this list, or at least one unit on [`PrerequisiteProcAlternate`](/keys/prerequisiteprocalternate/). Either one is enough, and the order of the list does not matter. A structure counts from the moment it is placed until it is taken off the map or captured.

`PROC` is the refinery requirement. In the standard rules the Ore Refineries are listed here, so a War Factory that names `PROC` cannot be built before the house owns a refinery.

A computer house's [production](/systems/production/#computer-houses) ignores prerequisites. Its [base planner](/systems/ai-base-building/#building-the-plan) treats `PROC` as met once the first refinery it can own, from [`BuildRefinery`](/keys/buildrefinery/), is in the plan.

Write BuildingType IDs; case does not matter. A name that matches no BuildingType never counts. If the list is empty and `PrerequisiteProcAlternate` lists no owned unit, `PROC` can never be satisfied.
