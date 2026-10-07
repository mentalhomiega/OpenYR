---
key: PrerequisiteProcAlternate
summary: The UnitTypes that satisfy a PROC prerequisite in place of a refinery.
see_also: [PrerequisiteProc, "system:production"]
when_omitted:
  kind: value
  value: ""
---

A house satisfies a `PROC` entry in a [`Prerequisite=`](/keys/prerequisite/) list by owning at least one unit of any type on this list, as it does with a structure on [`PrerequisiteProc`](/keys/prerequisiteproc/). The standard rules list the Slave Miner here, so a Yuri house that has packed its refinery up into a Slave Miner keeps the types that need a refinery.

Write UnitType IDs; case does not matter. A name that matches no UnitType is dropped.
