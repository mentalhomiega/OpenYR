---
key: TaskForce
summary: The TaskForce whose composition a team of this type recruits and builds toward.
see_also: [Script, House, Group, Max, "system:ai-team-production"]
when_omitted:
  kind: computed
  note: The first TaskForce that an earlier TeamType names with `TaskForce=`, or none when no earlier TeamType names one.
---

The named TaskForce is the team's roster. Each of its member lines names an object type and a quantity. A team [recruits](/systems/ai-team-production/#recruitment) toward those quantities, and a computer house builds the members still missing. The team is at full strength when it holds the sum of the quantities. The [Reinforcement (team)](/mapping/actions/taction-reinforcements/) and [Reinforcement (team) at waypoint](/mapping/actions/taction-reinforcements-special/) actions create exactly those members instead of recruiting them. A TaskForce of a single `DSHP` line is the exception, because it can [deliver a dropship loadout](/keys/startingdropships/) instead.

When the TeamType leaves its own [`Group`](/keys/group/#scope-teamtype) unset, the team recruits with the TaskForce's [`Group`](/keys/group/#scope-taskforce).

```ini title="ai.ini or map file"
[MyRaidTeam] ; example TeamType
Name=Raid team
House=Nod
TaskForce=MyRaidForce ; defined under [TaskForces]
Script=MyRaidScript   ; defined under [ScriptTypes]
```

A TeamType with no `TaskForce=`, or with `<none>` or `none`, takes the first TaskForce that an earlier TeamType names. Load order reads the AI's TeamTypes first, then the map's, each in `[TeamTypes]` order. The `[TaskForces]` section is read after every TeamType, so a TaskForce listed only there does not count. A TeamType keeps no TaskForce while no earlier TeamType has named one.

The value is matched against the IDs registered under `[TaskForces]`. A name that is not registered there is not an error: the game creates a new, empty TaskForce under that name. What happens next depends on how the team is created:

- Either reinforcement action creates nothing.
- Any other team wants no members, so it never reaches full strength and never starts its Script. Outside a campaign it is deleted once [`DissolveUnfilledTeamDelay`](/keys/dissolveunfilledteamdelay/) frames have passed since it was created. In a campaign it stays for the rest of the mission.

In a Debug build, a team with an empty TaskForce stops the game on a failed assertion the first time its logic runs.

:::danger[Name a TaskForce before a TeamType without one]
A TeamType keeps no TaskForce when its `TaskForce=` is missing, `<none>` or `none` and no earlier TeamType has named one. The game then crashes:

- An `[AITriggerTypes]` entry that names such a TeamType crashes the game while the scenario loads.
- Either reinforcement action crashes before it creates anything.
- A [Change team...](/mapping/missions/tmission-teamchange/) mission that names such a TeamType crashes as soon as it runs.
- A team created by a [Create Team](/mapping/actions/taction-create-team/) action crashes on its first logic turn.
:::
