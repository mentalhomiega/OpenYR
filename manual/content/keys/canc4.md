---
key: CanC4
summary: "Lets airstrikes and demolition charges be aimed at this structure."
see_also: [Airstrike]
when_omitted:
  kind: value
  value: "yes"
---

With `CanC4=no`, a unit with an [`Airstrike`](/keys/airstrike/) second weapon does not choose that weapon against the structure.

A player's soldier with [`C4=yes`](/keys/c4/) or the `C4` ability does not plant a demolition charge on a structure with `CanC4=no`; it attacks the structure. A computer-owned one walks into it, as an [`Infiltrate=yes`](/keys/infiltrate/) soldier does.

A structure with `CanC4=no` also takes at least one point from each hit, even one that its armor and the warhead's [`Verses`](/keys/verses/) reduce to nothing. A structure with the default `CanC4=yes` takes no damage from such a hit.

```ini title="rulesmd.ini"
[MYBUNKER] ; example BuildingType
CanC4=no
```
