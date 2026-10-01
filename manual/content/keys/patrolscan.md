---
key: PatrolScan
summary: How often a patrolling team looks around for something to attack.
see_also: ["system:target-selection"]
when_omitted:
  kind: value
  value: ".016"
---

The value is in minutes and becomes a whole number of frames, so the default is 14 frames at 900 frames to the game minute. Only the [Patrol to waypoint...](/mapping/missions/tmission-patrol/) team mission reads it.

At each interval, a patrolling team looks for the greatest threat within its leader's [`GuardRange`](/keys/guardrange/), or within the leader's weapon range when `GuardRange` is zero. If it finds one, the team attacks it. If it finds none, the team drops any target it was attacking and continues toward its waypoint.

The leader is the member with the highest [`LeadershipRating`](/keys/leadershiprating/) among those that are alive, out of [limbo](/glossary/#limbo), and [in formation](/systems/ai-team-execution/#bringing-a-member-into-formation) or an aircraft. A tie goes to the most recently joined of them, and when no member qualifies the most recently joined member leads.

The interval counts game frames, not the time since each team started patrolling, so every patrolling team in the scenario scans on the same frames.

:::danger[Keep the interval at one frame or more]
`PatrolScan=0`, or any value closer to zero than `1/900` (about `0.0011`), gives an interval of zero frames. The game then crashes with a divide by zero the first time any team runs a Patrol mission.
:::
