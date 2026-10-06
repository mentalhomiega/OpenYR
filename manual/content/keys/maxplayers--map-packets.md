---
key: MaxPlayers
scope: map-packets
label: Packet-listed player ceiling
see_also: [MinPlayers, Description]
when_omitted:
  kind: value
  value: "4"
---

```ini title="MyMaps.PKT"
[MYMAP] ; a section named by the packet's [MultiMaps] list
MinPlayers=2
MaxPlayers=4
```

For a map listed by a [map pack](/formats/map-packs/), the map list shows the value after the map's name, as in `Four player canyon (2-4)`. Nothing else reads it. No lobby caps the number of players at it, and starting positions and houses are assigned without it. A map declaring `MaxPlayers=4` still accepts as many players as the lobby allows.
