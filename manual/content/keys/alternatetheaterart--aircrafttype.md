---
key: AlternateTheaterArt
scope: aircrafttype
summary: "Makes the type draw the art section that carries its theater's image letter."
see_also: [Image, NewTheater, ImageLetter]
when_omitted:
  kind: value
  value: "no, so the type always draws its own art section"
---

With `AlternateTheaterArt=yes`, a soldier, vehicle, aircraft or structure type looks for an art section named after its [`Image`](/keys/image/) name followed by the [`ImageLetter`](/keys/imageletter/) of the map's theater. With `Image=JUNK` on a map whose theater has the letter `A`, the type reads `[JUNKA]` and draws `JUNKA.SHP`. When the art file has no such section the type keeps its own, and so does a type whose own section sets `Voxel=yes`.

The theater's section replaces the type's art section completely, so it must repeat every art key the type needs.

```ini title="artmd.ini"
[GGI] ; example art section
Cameo=GGICON

[GGIA] ; used on maps of a theater with the letter A
Cameo=GGICON
```

```ini title="rulesmd.ini"
[GGI] ; example InfantryType
AlternateTheaterArt=yes
```
