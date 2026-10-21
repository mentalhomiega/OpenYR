---
key: Palette
summary: "Names a palette that structure, parachute and grapple animations are drawn through."
see_also: [CustomPalette, AltPalette, ShouldUseCellDrawer, TerrainPalette, Parachute]
when_omitted:
  kind: value
  value: "none; the animations are drawn in their house's colors"
---

The value is a palette name without an extension, such as `lib`. The game adds the current map's theater suffix and `.pal`, so `lib` reads `libtem.pal` on a temperate map. The name is not case sensitive.

A structure with `Palette=` draws each of its animations that use the [cell drawer](/keys/shouldusecelldrawer/), and its destroy animation, through that palette, in its house's colors. Its body stays in its house's colors.

A vehicle, infantryman or aircraft that sets `Palette=` has its [parachute](/keys/parachute/) canopy drawn through that palette, in its house's colors. We draw a giant squid's grapple through the squid's own palette. Only the canopy and the grapple take the palette on these objects; their bodies stay in their house's colors.

If the file is not found, the animations above keep their house colors, and the debug log names the file.

The palette takes the map's lighting. The airstrike and Force Shield tints do not reach a structure's animations.

```ini title="artmd.ini"
[MYSTATUE] ; example BuildingType art section
Palette=lib
```
