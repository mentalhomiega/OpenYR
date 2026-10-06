---
key: TransportWaypoint
scope: teamtype
label: Transport plane start waypoint
see_also: [UseTransportOrigin, Droppod]
when_omitted:
  kind: value
  value: "none"
---

`TransportWaypoint` names the waypoint, by its letters such as `B` or `AC`, where the paradrop plane of a [`Droppod=yes`](/keys/droppod-teamtype/) team starts. It is used only together with [`UseTransportOrigin=yes`](/keys/usetransportorigin/).

```ini title="ai.ini or map file"
[MyDropTeam] ; example TeamType
Droppod=yes
UseTransportOrigin=yes
TransportWaypoint=B
```
