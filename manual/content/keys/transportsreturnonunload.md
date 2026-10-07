---
key: TransportsReturnOnUnload
summary: Releases each transport from the team when an Unload mission finishes and puts it on the Move mission.
see_also: [Passengers, "system:ai-team-execution"]
when_omitted:
  kind: value
  value: "no"
---

With this set, once an [Unload](/mapping/missions/tmission-unload/) mission has emptied every transport, each transport leaves the team and is put on the Move mission. A transport is a member whose type sets [`Passengers`](/keys/passengers/) above zero. When the team's TaskForce includes an aircraft that carries passengers, only aircraft count as transports, and a ground transport is treated like any other member.

The Unload mission's argument, the number on its script line, still decides what happens to the members that are not transports. It has no effect on the transports, which are released whatever it names.

The transport drives back to a return point the team keeps for it:

1. A [Load onto Transport](/mapping/missions/tmission-load/) line that finishes records the cell its transport stands in. Each time the team moves toward a target, every transport that has no return point yet records the cell it stands in.
2. When the team advances to the next line of its script, it erases the return point of every member except a transport.
3. On release, the transport is sent to its return point, and the point is cleared.

A transport that never loaded or moved with the team has no return point. With no destination, the Move mission sends it straight to its idle behavior.
