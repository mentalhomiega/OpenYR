---
key: Description
scope: multiplayer-maps
label: Loose map list entry
see_also: [MinPlayers, MaxPlayers, Name, Official]
when_omitted:
  kind: computed
  note: The title from the map's [Basic] Name, including the fallback that key applies when Name is missing as well.
---

A loose `.YRM` or `.MPR` map, one found on its own in the searched folders and not listed in a packet, is shown under this text in the multiplayer scenario list. At most 43 bytes are kept; a longer value is cut. An empty value counts as a missing one, so the row falls back to the map's [`Name`](/keys/name/#scope-multiplayer-maps).

```ini title="MyMap.MPR"
[Multiplay]
Description=Four player canyon
```

In the game's own network lobby, the host sends this text to the other players with the game options. Each comma in it is sent as `%2C` and shown as a comma. A joining player who has a map with the same file name sees the text their own list gives it. The host's text is shown only to a player who does not have the map.
