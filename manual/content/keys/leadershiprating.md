---
key: LeadershipRating
summary: How strongly an object is preferred as the leader of the AI team it belongs to.
see_also: ["system:ai-team-execution"]
when_omitted:
  kind: value
  value: "5"
---

A team's leader is the member with the highest [`LeadershipRating`](/keys/leadershiprating/) among those that are alive, out of [limbo](/glossary/#limbo), and [in formation](/systems/ai-team-execution/#bringing-a-member-into-formation) or an aircraft. A tie goes to the most recently joined of them, and when no member qualifies the most recently joined member leads. The leader picks targets and destinations for several script lines, such as Attack, Move to waypoint, Patrol and the two gather lines.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
LeadershipRating=8
```
