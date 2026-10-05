---
key: UseTransportOrigin
scope: teamtype
label: Start the transport plane at a waypoint
see_also: [TransportWaypoint, Droppod]
when_omitted:
  kind: value
  value: "no"
---

`UseTransportOrigin=yes` makes the paradrop plane of a [`Droppod=yes`](/keys/droppod-teamtype/) team start at the team's [`TransportWaypoint`](/keys/transportwaypoint/) instead of entering from the house's map edge. Without a `TransportWaypoint`, the plane enters from the map edge as usual.

```ini title="ai.ini or map file"
[MyDropTeam] ; example TeamType
Droppod=yes
UseTransportOrigin=yes
TransportWaypoint=B
```
