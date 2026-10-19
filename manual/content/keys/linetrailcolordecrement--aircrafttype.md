---
key: LineTrailColorDecrement
scope: aircrafttype
label: Line trail fade step
see_also: [UseLineTrail, LineTrailColor]
when_omitted:
  kind: value
  value: "16"
---

The opacity a position of the line that [`UseLineTrail=yes`](/keys/uselinetrail/) gives the type loses each frame, from 255. A position is clear after `255 / LineTrailColorDecrement` frames, rounded up, and a clear position ends the line. With the default of `16` a position lasts 16 frames, and with the detail level on Low the step is doubled.

:::caution[Keep the value above 0]
A value of `0` or less never fades the line. The line then shortens only as new positions replace the old ones, and the line of an object that has been destroyed stays on the map.
:::
