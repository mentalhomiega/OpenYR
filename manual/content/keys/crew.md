---
key: Crew
summary: The InfantryType that leaves a destroyed or sold crewed object owned by a house of the side.
see_also: ["system:capture", AlliedCrew, SovietCrew, ThirdCrew, Crewed, "Technician"]
when_omitted:
  kind: computed
  note: '`AlliedCrew`, `SovietCrew` or `ThirdCrew`, for the first, second or third side in `[Sides]`; a side after the third leaves the side technician.'
---

This key replaces the `[General]` crew type for one side. It names the type a [`Crewed=yes`](/keys/crewed/) structure or vehicle leaves as an ordinary survivor when a house of the side owns it. The tests that decide between the crew type, the technician and the engineer are in [`AlliedCrew`](/keys/alliedcrew/).

```ini title="rules.ini"
[FourthSide]
Crew=KNIGHT
```

The type is read by name when the rules are read, and a name that no section defines registers a new type that has only engine defaults.
