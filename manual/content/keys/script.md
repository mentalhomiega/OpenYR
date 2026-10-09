---
key: Script
summary: The Script whose team missions a team of this type carries out.
see_also: [TaskForce, House, Tag, "system:ai-team-production"]
when_omitted:
  kind: computed
  note: The first Script that an earlier TeamType names with `Script=`, or none when no earlier TeamType names one.
---

A team built from this TeamType carries out the named Script's team missions one line at a time. When it runs past the last line, the team is disbanded and its members stay on the map. Each team keeps its own place in the Script, so several teams of the same type can be on different lines at once.

```ini title="ai.ini or map file"
[MyRaidTeam] ; example TeamType
Name=Raid team
House=Nod
TaskForce=MyRaidForce ; defined under [TaskForces]
Script=MyRaidScript   ; defined under [ScriptTypes]
```

The value is matched against the IDs registered under `[ScriptTypes]`. A name that is not registered there is not an error. The game creates a new, empty Script under that name, so a misspelled name gives the team no missions. A team with an empty Script is disbanded when it tries to run its first line. The [reinforcement actions](/mapping/actions/taction-reinforcements/) are the exception: a TeamType with no Script gets a new, empty one, and a Script with no missions gets a Guard area mission with a timer of zero, which ends at once. That mission stays in the Script for every later team that uses it.

A TeamType with no `Script=`, or with `none` or `<none>`, takes the first Script that an earlier TeamType names. Load order runs the AI's TeamTypes first, then the map's, each in `[TeamTypes]` order. A Script listed under `[ScriptTypes]` is registered after every TeamType is read, so the map's own list is used only when an earlier TeamType has named that Script. The Script stays on the TeamType, so every later team of that type runs the same missions.

:::danger[Give every TeamType a Script]
A TeamType with no Script keeps none in two cases. The first is a TeamType with no `TaskForce=` while no earlier TeamType has named a TaskForce. The second is one while no earlier TeamType has named a Script. A team made from a TeamType with no Script crashes the game when it runs its first mission. The one exception is a scenario in which no Script is registered at all: there each such TeamType gets a Guard-only Script at load, so its teams are disbanded at once. Name a Script on every TeamType whose teams should do anything.
:::
