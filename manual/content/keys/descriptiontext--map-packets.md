---
key: DescriptionText
scope: map-packets
label: Packet list entry text
see_also: [Description, MinPlayers, MaxPlayers]
when_omitted:
  kind: computed
  note: The row shows the text that the section's Description gives it.
---

The value is the map's row text in the multiplayer scenario list, shown as written. It takes the place of [`Description`](/keys/description/#scope-map-packets), which names a string table label, so a packet can name a map without a string table of its own. At most 43 bytes of the row text are kept; a longer text is cut. An empty value counts as a missing one.

```ini title="MyMaps.PKT"
[MultiMaps]
1=MYMAP

[MYMAP] ; the section the entry above names
DescriptionText=Four player canyon
```

A map listed in a [map pack](/formats/map-packs/) shows its player limits after this text.
