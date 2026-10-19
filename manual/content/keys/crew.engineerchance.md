---
key: Crew.EngineerChance
summary: The odds in percent that a survivor of this structure type is an engineer.
see_also: ["Crew.TechnicianChance", Crewed, Engineer, "system:capture"]
when_omitted:
  kind: computed
  note: 25 for a structure that builds buildings, otherwise 0.
---

`Crew.EngineerChance` is a whole number of percent from `0` to `100`. Each survivor of a destroyed or sold structure of this type rolls it first, and a success makes the survivor the `[General]` [`Engineer`](/keys/engineer/#scope-global-rules) type. A structure that was ever captured releases no engineer.

When the key is absent, a structure that builds buildings rolls 25 percent, and any other structure never makes an engineer. A sale releases at most one engineer even at `100`; its other survivors take the technician or crew roll.

```ini title="rulesmd.ini"
[GAPILE] ; example BuildingType
Crewed=yes
Crew.EngineerChance=0 ; no engineers
```
