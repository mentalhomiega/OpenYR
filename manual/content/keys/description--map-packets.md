---
key: Description
scope: map-packets
label: Packet list entry
see_also: [MinPlayers, MaxPlayers]
when_omitted:
  kind: value
  value: ""
  note: The map is listed with a blank description.
---

The value names the map's row in the multiplayer scenario list. It is a [string table](/formats/csf/) label, such as `DESC:MP29U2`, and the row shows that label's text. A value that matches no label is shown as written. At most 43 bytes of the row text are kept; a longer text is cut.

A scenario packet lists its maps in `[MultiMaps]`. Each value there names a section of the packet, and that section holds the map's settings, this key included.

```ini title="MyMaps.PKT"
[MultiMaps]
1=MYMAP

[MYMAP] ; the section the entry above names
Description=Four player canyon
```

The list starts with the maps in `MISSIONSMD.PKT`, which the game's archives hold. Every other `.PKT` file found on its own in the searched folders adds its maps after them.

The map file is the section name with `.MAP` appended, `MYMAP.MAP` in this example. When it builds the list, the game reads that file only for its digest, which other players' copies are checked against, and never for a description.

In the game's own network lobby, the host sends the row text to the other players with the game options, with each comma in it replaced by a semicolon. A joining player who has a map with the same file name sees the text their own list gives it. The host's text is shown only to a player who does not have the map.

:::caution[Give every packet map a description]
The game options travel as a comma-separated list. An empty description makes each joining player misread the fields after it, including the map's file name, so the map is not matched.
:::
