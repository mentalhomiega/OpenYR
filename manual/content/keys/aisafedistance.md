---
key: AISafeDistance
summary: How many cells from a base center a team gathers before or after an attack.
see_also: ["LeadershipRating", "system:ai-team-execution"]
when_omitted:
  kind: value
  value: "8"
---

The [Gather at enemy base](/mapping/missions/53/) and [Regroup at friendly base](/mapping/missions/54/) script lines send a team to a spot this many cells from a base center, along the line toward the other base. The spot then moves to the nearest clear area the team's leader can stand in.

```ini title="rulesmd.ini"
[General]
AISafeDistance=10
```
