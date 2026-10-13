---
key: Factory
summary: The kind of object the BuildingType produces.
see_also: ["system:production"]
when_omitted:
  kind: value
  value: "none"
---

Write `UnitType`, `InfantryType`, `AircraftType` or `BuildingType` to make the structure produce vehicles, infantry, aircraft or structures. The name is matched without regard to case.

```ini title="rules.ini"
[MYWEAP] ; example war factory BuildingType
Factory=UnitType
```

A player's house has one production slot for each of these kinds: infantry, vehicles, ships, aircraft and structures. A ship is a vehicle type with [`Naval=yes`](/keys/naval/), and it has a slot apart from the vehicles. A defense is a `BuildingType` whose [`BuildCat=`](/keys/buildcat/) is `Combat`, and it has a slot of its own too. Every other structure that builds the same kind shares that kind's slot, so they build one object at a time between them. A second such structure can shorten build times instead, depending on [`MultipleFactory`](/keys/multiplefactory/); see [More than one factory](/systems/production/#more-than-one-factory).

A computer house has no shared slot. Each of its factories builds separately, choosing whatever the house wants next of that kind.

The short names `Unit`, `Infantry`, `Aircraft` and `Building` also parse, but only a computer house's structures produce with them. A player's structure with a short name never produces anything, although it still counts toward the multiple-factory speed-up. Any other value, including an unrecognized one, leaves the structure producing nothing. [What counts as a factory](/systems/production/#what-counts-as-a-factory) covers how a player's order picks among the house's factories.
