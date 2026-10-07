---
key: RelaxedStray
summary: Distance in cells a team member may drift from the team while the team gathers.
see_also: [Stray, AISafeDistance, "system:ai-team-execution"]
when_omitted:
  kind: value
  value: "2"
  note: "512 leptons."
---

`RelaxedStray` replaces [`Stray`](/keys/stray/) in every test that [`Stray`](/keys/stray/) governs while the team's current Script line is [Gather at enemy base](/scripting/missions/53/) or [Regroup at friendly base](/scripting/missions/54/). A larger value lets a big team spread out around its gathering spot without members being called back. The value is in cells, and fractions are accepted.

On every other line the team uses `Stray`.

```ini title="rulesmd.ini"
[General]
RelaxedStray=3 ; gathering teams may spread over 3 cells
```
