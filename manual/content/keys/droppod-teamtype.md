---
key: Droppod
summary: Makes a reinforcement TeamType fly in aboard a paradrop plane and parachute over its waypoint.
see_also: [UseTransportOrigin, TransportWaypoint, Waypoint]
when_omitted:
  kind: value
  value: "no"
---

`Droppod=yes` makes a reinforcement team arrive by air. The game creates a `PDPLANE` for the team's house, loads every member of the team aboard it, and sends it to paradrop them over the team's [`Waypoint`](/keys/waypoint/), or over the waypoint the trigger action names. Despite the key's name, no drop pods are used.

The plane enters from the house's map edge, or from the north edge when the house names none. With [`UseTransportOrigin=yes`](/keys/usetransportorigin/), it starts at the team's [`TransportWaypoint`](/keys/transportwaypoint/) instead. The team arrives this way only when it is delivered by the [Reinforcement (team)](/mapping/actions/taction-reinforcements/) or [Reinforcement (team) at waypoint](/mapping/actions/taction-reinforcements-special/) trigger action; teams the AI builds and recruits are not flown in.

When the map has no `PDPLANE` aircraft type, or the plane cannot be placed, the team does not arrive.

```ini title="ai.ini or map file"
[MyDropTeam] ; example TeamType
Droppod=yes
TaskForce=MyInfantryTaskForce ; defined under [TaskForces]
Waypoint=A
```

The `[AudioVisual]` animation list is a different key, spelled [`DropPod`](/keys/droppod-global-rules/) with an uppercase second `P`.
