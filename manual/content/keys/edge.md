---
key: Edge
summary: The map edge that a scenario house's reinforcement teams and support planes enter from.
see_also: [PlayerControl, Credits]
when_omitted:
  kind: computed
  note: North for ground reinforcements. For planes, the edge of the local area nearest the house's construction yard in a campaign; outside a campaign, North.
---

`Edge` sets the map edge where the house's [reinforcement teams](/mapping/actions/taction-reinforcements/) enter the map, the direction they face when they appear on a waypoint, and the edge its planes fly in from. The value is one of the [reinforcement sources](/reference/enums/reinforcement-source/).

Only a campaign mission reads its house records, so this is a campaign setting. Outside a campaign, every house keeps the default, North.

```ini title="scenario map file"
[Nod] ; a house record in the scenario's own house list
Edge=South
```

When a campaign house record omits `Edge`, ground reinforcements enter from the north edge, and planes enter from the edge of the local area that the house's construction yard lies nearest. That edge is set when a construction yard is placed, captured or made the primary factory, and destroying the yard does not change it. With several construction yards, the primary one counts.

The TeamType's [`Waypoint`](/keys/waypoint/) decides where the team appears:

- If the TeamType names no waypoint, the team enters at a clear cell along this edge. A ground team uses the north edge when the value is one the engine does not recognize, or `Air`. A team of aircraft instead uses the planes' edge described above.
- If it names a waypoint, the team appears on that waypoint's cell and faces this edge.

Teams that arrive by drop pod, by burrowing, by walking out of a structure or transport, or through the [Reinforcement (team) at waypoint](/mapping/actions/taction-reinforcements-special/) action do not cross the map edge, so this setting does not affect them.

The Retreat mission uses this setting only for planes. A plane on Retreat heads for the edge its planes fly in from. Infantry and vehicles on that mission head for the map edge nearest their team's waypoint, or nearest their own position when there is no team waypoint.
