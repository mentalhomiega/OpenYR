---
key: TeamRetaliate
summary: Whether an idle computer team turns on whoever hurts one of its members.
see_also: ["system:ai-base-building", TeamDelays]
when_omitted:
  kind: value
  value: "the engine's own response"
---

`TeamRetaliate` is an Ares key, read from `[General]`. `TeamRetaliate=yes` makes a team that is not on the move take the attacker as its target when a member is hurt and not killed outright, and its members drop their own targets to follow. An attacker that is an aircraft, a type with `ConsideredAircraft=yes`, or a parasite inside a victim is ignored. A team that already has a target keeps it when a second attacker strikes.

`TeamRetaliate=no` plays the response of the original game, in which a team never takes the attacker as its target. A team with `Annoyance=yes` still breaks off and regroups.

Without the key the game keeps its own response, inherited from Tiberian Sun: the team takes the attacker as its target, ignoring aircraft but not parasites, and keeps a target only when that target can fire back and is within range of the team's zone.

```ini title="rulesmd.ini"
[General]
TeamRetaliate=yes
```
