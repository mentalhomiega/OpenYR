---
key: SovietCrew
summary: The InfantryType that ordinarily leaves a destroyed or sold object owned by a house of the second side.
see_also: ["system:capture", AlliedCrew, ThirdCrew, Crewed, Technician]
when_omitted:
  kind: value
  value: none
---

Only a [`Crewed=yes`](/keys/crewed/) structure or vehicle produces survivors. [Survivors](/systems/capture/#survivors) covers how many a structure produces, and [`CrewEscape`](/keys/crewescape/) covers a vehicle's crew. The type of each survivor is chosen separately, by these tests in order:

1. A structure that has never been captured and whose type builds structures produces the `[General]` [`Engineer`](/keys/engineer/#scope-global-rules) type on a one-in-four roll.
2. An object whose house belongs to no side produces the [`Technician`](/keys/technician/) type.
3. An armed object whose house has a side produces the `Technician` type on a 15% roll.
4. Any other survivor is of the crew type for the side of the object's own country: `AlliedCrew` for the first side in `[Sides]`, `SovietCrew` for the second and `ThirdCrew` for the third. A house of a later side leaves the `Technician` type.

A structure whose house is on no side, or on a side after the third, releases no survivors. Step 2 and the later-side case of step 4 therefore apply only to vehicles.

```ini title="rulesmd.ini"
[General]
SovietCrew=E2
```

The value does not have to be listed under `[InfantryTypes]`. A name the game does not already know registers a new InfantryType, and a section of that name is still read, because object sections are read after `[General]`. A name with no section of its own gives a type with only engine defaults and no animation sequence, and a survivor of that type crashes the game once it animates.

With no type named, a destroyed object produces no survivor for a pick that falls through to step 4, and a sale stops producing survivors at the first such pick.
