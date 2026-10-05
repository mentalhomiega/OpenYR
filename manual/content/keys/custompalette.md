---
key: CustomPalette
summary: Names a palette file that the animation is drawn through instead of the shared animation palette.
see_also: [AltPalette, Tiled, ShouldUseCellDrawer, UseNormalLight]
when_omitted:
  kind: value
  value: "none; the animation is drawn through the shared animation palette"
---

The value is a palette file name with its extension, such as `anim.pal`. The animation's colors come from that file, and the brightness the animation is lit with is unchanged. Three tildes in the name, as in `anim~~~.pal`, are replaced by the current map's theater suffix, so one assignment picks a different file per theater. The file is searched for in the loaded MIX archives, and the name is not case sensitive.

The setting replaces the shared animation palette and takes precedence over [`AltPalette=yes`](/keys/altpalette/). It applies only to an animation that nothing else gives a palette, so these ignore it:

- an animation that is attached to a cell, such as one started by a terrain tile
- an animation with [`IsVeins=yes`](/keys/isveins/#scope-animtype)
- an animation drawn in a player's, an owner's or Tiberium's colors, such as a structure's animation unless it sets [`ShouldUseCellDrawer=no`](/keys/shouldusecelldrawer/)
- a [`Tiled=yes`](/keys/tiled/) animation

If the file is not found, the animation is drawn through its normal palette and one line naming the file is written to the debug log. Projectiles have no such setting.

```ini title="artmd.ini"
[MYFLASH] ; example AnimType
CustomPalette=flash~~~.pal ; flashtem.pal on a temperate map
```
