---
key: LineTrailColorOverride
scope: global-rules
label: Line trail color for every type
see_also: [LineTrailColor, UseLineTrail]
when_omitted:
  kind: value
  value: "0,0,0"
---

`LineTrailColorOverride=` in `[AudioVisual]` gives every [line trail](/keys/uselinetrail/) one color, whatever [`LineTrailColor`](/keys/linetrailcolor/) says. Three comma-separated numbers from `0` to `255`, for red, green and blue. The value `0,0,0` turns the override off, so no line trail can be made black this way. A line takes the color when its object is placed on the map, so changing the value does not recolor lines that already exist.
