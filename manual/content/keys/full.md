---
key: Full
summary: Fills the storage of every member created for a reinforcement group of this type, and lets the group's members with passenger room carry the rest.
see_also: [Storage, VeteranLevel, "system:tiberium"]
when_omitted:
  kind: value
  value: "no"
---

`Full=yes` makes each reinforcement member whose type sets [`Storage`](/keys/storage/) above zero start with a full load: its whole `Storage` amount of the first Tiberium type in the rules. A member whose type has no storage is unaffected.

```ini title="ai.ini or map file"
[MyHarvesterDelivery] ; example TeamType
Name=Loaded harvesters
TaskForce=MyHarvesterForce ; defined under [TaskForces]
Script=MyHarvesterScript   ; defined under [ScriptTypes]
Full=yes
```

Only the [Reinforcement (team)](/mapping/actions/taction-reinforcements/) and [Reinforcement (team) at waypoint](/mapping/actions/taction-reinforcements-special/) trigger actions apply the setting, because they create the team's members from its TaskForce. [`VeteranLevel`](/keys/veteranlevel/) is applied at the same moment. A team that fills its roster by [recruiting](/systems/ai-team-production/#recruitment) objects its house already owns never applies it.

The load is given once, when the member is created. The setting does not refill a member later.

`Full=yes` also lets the team's members with passenger room carry its other members. [Reinforcement (team)](/mapping/actions/taction-reinforcements/) gives the rules for which members are transports. Without `Full`, no member is a transport or a passenger, so each one arrives on its own.
