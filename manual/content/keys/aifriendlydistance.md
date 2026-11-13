---
key: AIFriendlyDistance
summary: How many cells from its own base center a team gathers on a Regroup at friendly base line.
see_also: [AISafeDistance, "LeadershipRating", "system:ai-team-execution"]
when_omitted:
  kind: computed
  note: "The value of AISafeDistance."
---

`AIFriendlyDistance` is an Ares key, read from `[General]`. The [Regroup at friendly base](/mapping/missions/tmission-gather-at-base/) script line sends a team to a spot this many cells from the center of its own base, along the line toward its enemy's base. The line's parameter is added to the distance, so one value serves every team and a script moves the spot by its own amount. A negative total puts the spot on the far side of the base center. The spot then moves to the nearest clear area the team's leader can stand in.

[`AISafeDistance`](/keys/aisafedistance/) still sets the distance of the [Gather at enemy base](/mapping/missions/tmission-gather-at-enemy/) line.

```ini title="rulesmd.ini"
[General]
AIFriendlyDistance=4
```
