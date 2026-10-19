---
key: Crew.TechnicianChance
summary: The odds in percent that a survivor of this type is its owner's technician rather than its side's crew.
see_also: ["Crew.EngineerChance", Crewed, Technician, "Survivor.Side0"]
when_omitted:
  kind: computed
  note: An armed object has 15 percent odds, and an unarmed one none.
---

`Crew.TechnicianChance` is a whole number of percent from `0` to `100`. Each survivor of a destroyed or sold object of this type rolls it, and a success makes the survivor the [`Technician`](/keys/technician/) type. A survivor that fails is the owning side's crew type.

When the key is absent, an object with a weapon rolls 15 percent and one without a weapon never does. A house with no side always releases technicians. A type that names a survivor with [`Survivor.Side0`](/keys/survivor.side0/) or its siblings for the owner's side does not roll.

```ini title="rulesmd.ini"
[GAPOWR] ; example BuildingType
Crewed=yes
Crew.TechnicianChance=50
```
