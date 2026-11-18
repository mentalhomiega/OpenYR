---
key: ZoomOutView
scope: client-settings
label: Zoom out to a 4K view
when_omitted:
  kind: value
  value: "Original"
---

`ZoomOutView=` in the `[Video]` section of `RA2MD.INI` sets how much map the mouse wheel shows at its furthest zoom out. `Original` stops the zoom where the view shows no more map than the original game shows at 2560x1440. `4K` stops it where the view shows what the original shows at 3840x2160, the most the game allows. Another value reads as `Original`.

The limit holds in every game, multiplayer included, and a settings file or a mod cannot raise it past `4K`. The limit is on the map view in map pixels, so it does not depend on the interface scale: a 4K screen at `InterfaceScale=2` has a frame of 1920x1080 and stops at a zoom of 0.75, or 0.5 with `4K`. A frame no larger than the limit has no zoom out, and the zoom never goes below 0.5 times the start zoom, or 0.75 times it in a multiplayer game. In the classic interface the sidebar column and the command bar come off the view, as they do in the original at those sizes.

The [Map zoom](/systems/map-zoom/#how-far-out-the-zoom-goes) page gives the zoom this reaches on each screen size. [Scripted scenes](/systems/map-zoom/#scripted-scenes) are not held to the limit. With `DebugMode=yes` in a single-player game the limit does not apply.

The Graphics page shows the setting as the Zoom out to a 4K view switch, which picks `4K`.
