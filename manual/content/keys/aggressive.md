---
key: Aggressive
summary: Lets a member that holds a target stay and fight instead of closing on the team's destination.
see_also: [Stray, Suicide, "system:ai-team-execution"]
when_omitted:
  kind: value
  value: "no"
---

On an aggressive team, a member that holds a target keeps fighting while the rest of the team moves. When the team [moves](/systems/ai-team-execution/#moving), a member is normally ordered toward the team's target if it is too far from it, for example farther than [`Stray`](/keys/stray/). An aggressive team leaves such a member alone as long as it holds a target of its own.

The exempted member is not put on the Move mission and does not hold up the team's arrival. The rest of the team can arrive and start its next script line while that member stays where it is and keeps shooting. A member with no target is ordered toward the team's target as on any other team.

The setting affects only the move. Recruitment, the team's response to damage, and regrouping ignore it.

On a move order, a member of a computer's aggressive team that is not a [`Suicide=yes`](/keys/suicide/#scope-teamtype) team also scans while it holds no target. It takes the best target in its weapon's range whatever its type's [`OpportunityFire`](/keys/opportunityfire/) setting. Firing on the move follows the unit's usual rules, such as [`MobileFire`](/keys/mobilefire/) and [`NoMovingFire`](/keys/nomovingfire/). A human house's members do not scan this way.
