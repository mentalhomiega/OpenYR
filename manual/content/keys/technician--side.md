---
key: Technician
scope: side
label: Side technician type
see_also: ["system:capture", Crew, Crewed]
when_omitted:
  kind: computed
  note: The `[General]` `Technician` type.
---

A [`Crewed=yes`](/keys/crewed/) object that is armed and whose house belongs to the side leaves this type instead of its crew on a 15% roll for each survivor. The `[General]` key names the type for every side that sets none, and for a house that has no side.

```ini title="rules.ini"
[FourthSide]
Technician=CTECH
```
