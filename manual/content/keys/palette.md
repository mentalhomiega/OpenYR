---
key: Palette
summary: "Names a palette that a structure's animations and destroy animation are drawn through."
see_also: [CustomPalette, AltPalette, ShouldUseCellDrawer, TerrainPalette]
when_omitted:
  kind: value
  value: "none; the structure's animations are drawn in its house's colors"
---

The value is a palette name without an extension, such as `lib`. The game adds the current map's theater suffix and `.pal`, so `lib` reads `libtem.pal` on a temperate map. The name is not case sensitive.

A structure with `Palette=` draws each of its animations that use the [cell drawer](/keys/shouldusecelldrawer/), and its destroy animation, through that palette, in its house's colors. Its body stays in its house's colors. If the file is not found, those animations keep their house colors, and the debug log names the file.

The palette takes the map's lighting, but the airstrike and Force Shield tints do not reach these animations.

The key is read on every object type, but only structures use it. Parachute and grapple animations ignore it.

```ini title="artmd.ini"
[MYSTATUE] ; example BuildingType art section
Palette=lib
```
