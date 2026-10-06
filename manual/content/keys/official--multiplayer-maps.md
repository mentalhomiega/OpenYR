---
key: Official
scope: multiplayer-maps
label: Multiplayer map provenance
see_also: ["Name"]
when_omitted:
  kind: value
  value: "no"
---

`Official=yes` in the `[Basic]` section of a loose `.yrm` or `.mpr` map marks the map as one that shipped with the game. In a LAN game, the host's copy of the map decides the mark.

A guest cannot download a map marked official from the host. If the guest has no copy of the map with the same file name, size and `[Digest]` as the host's, it shows a message that it cannot play the map and leaves the game. A map without the mark is downloaded from the host instead.

Maps listed in a `.pkt` file or a [map pack](/formats/map-packs/) always count as official, whatever they set. The random map is always downloaded from the host, whatever its mark.
