---
key: Palette
summary: "Names a palette that a structure, and the parachute, grapple and deploying animations of other objects, are drawn through."
see_also: [CustomPalette, AltPalette, ShouldUseCellDrawer, TerrainPalette, Parachute, DeployingAnim]
when_omitted:
  kind: value
  value: "none; the object and its animations are drawn in their house's colors"
---

The value is a palette name without an extension, such as `lib`. The game adds the current map's theater suffix and `.pal`, so `lib` reads `libtem.pal` on a temperate map. The name is not case sensitive.

A structure with `Palette=` draws its body, each of its animations that use the [cell drawer](/keys/shouldusecelldrawer/), and its destroy animation through that palette, in its house's colors.

A vehicle, infantryman or aircraft that sets `Palette=` has its [parachute](/keys/parachute/) canopy drawn through that palette, in its house's colors. A vehicle that sets it also draws its [`DeployingAnim`](/keys/deployinganim/#scope-unittype) through the palette. We draw a giant squid's grapple through the squid's own palette. Their bodies stay in their house's colors.

If the file is not found, the animations above keep their house colors, and the debug log names the file.

The palette takes the map's lighting. The airstrike and Force Shield tints do not reach a structure's animations.

```ini title="artmd.ini"
[MYSTATUE] ; example BuildingType art section
Palette=lib
```
