---
key: Ring1Glow
summary: Whether an animation of type RING1 is drawn as an expanding glow on the ground instead of its shape.
see_also: [Rate, Translucent, DetailLevel]
when_omitted:
  kind: value
  value: "no"
---

At `yes`, an animation whose type ID is `RING1` is drawn as a glow on the ground and its shape is not drawn. The stock rules use `RING1` for the blasts of oil derricks, barrels, terror bombs and psychic waves, and for `IonBlast`. At `no`, `RING1` is drawn from its shape like any other animation.

The glow is a rectangle twice as wide as it is tall, centered on the animation. Its half-height is 8 pixels plus the number of game frames the animation has run, so it grows by one pixel each frame, and its half-width is twice that. The animation runs for its frame count times its [`Rate`](/keys/rate/#scope-animtype), and the glow ends then.

The glow is bright in the middle third of the rectangle and fades toward the edges. Its color changes over the run. Red starts at full and falls evenly to nothing. Green peaks a third of the way through, at two thirds of its full level. Blue peaks two thirds of the way through. The result runs from red through near white to blue, then dark. The glow is added to the picture beneath it, so it only brightens.

The glow is tested against the depth of what is already drawn, so a building, vehicle or other upright object standing in front of the center hides the part of the glow it covers. The glow is cut off at the edge of the tactical view.

The glow ignores [`Translucent`](/keys/translucent/#scope-animtype), [`DetailLevel`](/keys/detaillevel/#scope-animtype), and the fog and hidden-animation checks that the shape draw applies.

Yuri's Revenge has this drawing and runs it only when its Direct3D flag is set. Nothing in `gamemd.exe` sets that flag, so the shipped game draws the shape, which is what `no` does.

```ini title="rulesmd.ini"
[AudioVisual]
Ring1Glow=yes
```
