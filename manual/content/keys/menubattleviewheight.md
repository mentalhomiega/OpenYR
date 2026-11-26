---
key: MenuBattleViewHeight
summary: The height, in map pixels, that the main menu's background battle shows.
see_also: [ScriptedCameraHeight]
when_omitted:
  kind: value
  value: "1080"
---

`MenuBattleViewHeight` sets the zoom of the battle behind the main menu. The map view zooms so that this many map pixels fill its height, on a screen of any size, and its width follows the screen's aspect ratio. The default, `1080`, shows the same height of map as a 1920 by 1080 screen at normal size, so a 4K screen shows the map twice as large and the same area of it.

A value of `0` or less shows the battle at the screen's own pixels. The zoom stays within the limits that [Map zoom](/systems/map-zoom/) describes.
