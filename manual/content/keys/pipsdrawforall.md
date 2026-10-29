---
key: PipsDrawForAll
summary: "Shows a structure's pips to every player instead of only to its owner's allies."
see_also: [PipScale]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rulesmd.ini"
[MYPSYCHICSENSOR] ; example BuildingType
PipsDrawForAll=yes
```

The pips of a selected object, or of one under the mouse, normally show only to the players who share the owner's view and to a player who has spied on the owner. With `PipsDrawForAll=yes`, every player sees them on a structure of that type. The key does nothing on vehicles, soldiers and aircraft. A structure with [`CanBeOccupied=yes`](/keys/canbeoccupied/) shows its pips to everyone whatever this key says.
