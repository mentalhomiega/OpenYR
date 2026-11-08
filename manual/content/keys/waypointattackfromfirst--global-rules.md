---
key: WaypointAttackFromFirst
scope: global-rules
label: Attacks on a waypoint run its path first
summary: Lets an attack click on a waypoint start the path at its first waypoint, so the earlier waypoints run before the attack.
when_omitted:
  kind: value
  value: "yes"
---

`WaypointAttackFromFirst=yes` makes an attack click on a unit or building that stands on a [waypoint](/systems/waypoint-paths/) start the selected units' path at the path's first waypoint. The units drive through the earlier waypoints and attack when they reach the waypoint the target stands on.

`WaypointAttackFromFirst=no` starts the path at the clicked waypoint. The units attack at once and skip the waypoints before it.

A move click on a waypoint is not affected: it starts the path at the clicked waypoint either way.

The value is read from the rules, so every player in a multiplayer game must use the same rules.

Set it in the `[General]` section.
