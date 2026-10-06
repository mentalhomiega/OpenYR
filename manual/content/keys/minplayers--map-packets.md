---
key: MinPlayers
scope: map-packets
label: Packet-listed player minimum
see_also: [MaxPlayers, Description]
when_omitted:
  kind: value
  value: "2"
---

```ini title="MyMaps.PKT"
[MYMAP] ; a section named by the packet's [MultiMaps] list
MinPlayers=2
```

For a map listed by a [map pack](/formats/map-packs/), the map list shows the value after the map's name, as in `Four player canyon (2-4)`. Nothing else reads it. No lobby waits for this many players before a game can start, and starting positions and houses are assigned without it.
