---
key: TeamRetaliate
summary: Whether an idle computer team turns on whoever hurts one of its members.
see_also: ["system:ai-base-building", TeamDelays]
when_omitted:
  kind: value
  value: "no"
---

`TeamRetaliate` is an Ares key, read from `[General]`. `TeamRetaliate=yes` makes a team that is not on the move take the attacker as its target when a member is hurt and not killed outright, and its members drop their own targets to follow. An attacker that is an aircraft, a type with `ConsideredAircraft=yes`, or a parasite inside a victim is ignored. A team that already has a target keeps it when a second attacker strikes.

`TeamRetaliate=no`, the default, plays the response of Yuri's Revenge, in which a team never takes the attacker as its target. A team with `Annoyance=yes` still breaks off and regroups, whoever the attacker is.

```ini title="rulesmd.ini"
[General]
TeamRetaliate=yes
```
