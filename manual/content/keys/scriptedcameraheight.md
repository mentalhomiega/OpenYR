---
key: ScriptedCameraHeight
summary: The height, in map pixels, that a scripted scene's zoom shows.
see_also: [ScriptedCameraZoom, ZoomInFactor]
when_omitted:
  kind: value
  value: "900"
---

`ScriptedCameraHeight` sets the zoom of a scripted scene. The map view zooms so that this many map pixels fill its height, on a screen of any size, and its width follows the screen's aspect ratio. The default, `900`, shows the same height of map as a 1600 by 900 screen at normal size.

A value of `0` or less turns the scene zoom off, as [`ScriptedCameraZoom=no`](/keys/scriptedcamerazoom/) does. The zoom still stays within the limits that [Map zoom](/systems/map-zoom/) describes.
