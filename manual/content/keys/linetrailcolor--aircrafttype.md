---
key: LineTrailColor
scope: aircrafttype
label: Line trail color
see_also: [UseLineTrail, LineTrailColorDecrement, LineTrailColorOverride]
when_omitted:
  kind: value
  value: "128,128,128"
---

Three comma-separated numbers from `0` to `255`, for red, green and blue. They set the color of the line that [`UseLineTrail=yes`](/keys/uselinetrail/) gives the type, from the art section named by its [`Image`](/keys/image/). [`LineTrailColorOverride`](/keys/linetrailcoloroverride/) in `[AudioVisual]` replaces the color of every type when it is set.
