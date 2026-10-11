---
key: ExtraPower
summary: The power each object inside an absorbing structure adds to its output, or the extra drain of a charged gap generator.
see_also: [InfantryAbsorb, UnitAbsorb, SuperGapRadiusInCells, Power, "system:power"]
when_omitted:
  kind: value
  value: "0"
---

An [`InfantryAbsorb=yes`](/keys/infantryabsorb/) or [`UnitAbsorb=yes`](/keys/unitabsorb/) structure adds this much to its power output for each object inside it. The total is scaled by the structure's strength with the rest of its output, and a switched-off structure supplies nothing. [Power](/systems/power/) gives the whole calculation.

```ini title="rulesmd.ini"
[MYREACTOR] ; example BuildingType
InfantryAbsorb=yes
Passengers=5
ExtraPower=100 ; five soldiers inside add 500
```

A value of `0` adds nothing. The key has no effect on a structure that absorbs nothing.

A negative value is a drain instead. A [`GapGenerator=yes`](/keys/gapgenerator/) structure that its owner has charged with [`SuperGapRadiusInCells`](/keys/supergapradiusincells/) adds the size of the value to its drain until it is discharged. Other structures ignore a negative value.
