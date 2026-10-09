---
key: Script
summary: The Script whose team missions a team of this type carries out.
see_also: [TaskForce, House, Tag, "system:ai-team-production"]
when_omitted:
  kind: value
  value: none
---

A team built from this TeamType carries out the named Script's team missions one line at a time. When it runs past the last line, the team is disbanded and its members stay on the map. Each team keeps its own place in the Script, so several teams of the same type can be on different lines at once.

```ini title="ai.ini or map file"
[MyRaidTeam] ; example TeamType
Name=Raid team
House=Nod
TaskForce=MyRaidForce ; defined under [TaskForces]
Script=MyRaidScript   ; defined under [ScriptTypes]
```

The value is matched against the IDs registered under `[ScriptTypes]`. A name that is not registered there is not an error. The game creates a new, empty Script under that name, so a misspelled name gives the team no missions. A team with an empty Script is disbanded when it tries to run its first line. The [reinforcement actions](/mapping/actions/taction-reinforcements/) are the exception: they first add a Guard area mission with a timer of zero to an empty Script, which ends at once. That mission stays in the Script for every later team that uses it.

A TeamType with no Script gets a new Script with that one Guard mission the first time a team of it is made, whichever action or AI route makes it. The Script stays on the TeamType, so every later team of that type runs the same mission.

:::danger[Give every TeamType a Script]
`<none>`, `none` or a missing `Script=` gives the team only the Guard mission above, so the team is disbanded at once. Name a Script on every TeamType whose teams should do anything.
:::
