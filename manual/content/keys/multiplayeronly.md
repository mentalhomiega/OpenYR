---
key: MultiplayerOnly
summary: With a mod active, lists a loose map file in the skirmish and network map lists.
when_omitted:
  kind: value
  value: "no"
---

`MultiplayerOnly=yes` does not keep a map out of the campaign or limit it to multiplayer games. Yuri's Revenge reads the flag from the map's `[Basic]` section and never acts on it.

With a mod active, a loose `.MAP` file that sets `MultiplayerOnly=yes` is listed in the skirmish and network map lists, as the maps a mod ships for skirmish are. Without a mod, loose `.MAP` files are never listed, as in Yuri's Revenge.
