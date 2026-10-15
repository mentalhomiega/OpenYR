---
key: ScreenWidth
scope: client-settings-2
label: Width the display opens at
see_also: [ScreenHeight, WindowWidth, Fullscreen]
when_omitted:
  kind: computed
  note: A dimension left out keeps the size given on the command line, if any. A dimension still unset, or written as `-1`, becomes the primary display's desktop size.
---

The game opens its screen at `ScreenWidth` by [`ScreenHeight`](/keys/screenheight/#scope-client-settings-2) pixels. This read happens at startup, before the game window exists. After that, only the display options screen changes the size.

Writing `-1` for a dimension makes it the desktop size, even when the launch option gives one. An explicit size for the other dimension stays as written. If the desktop size cannot be read, a dimension that is unset or `-1` becomes 640 or 480 instead.

A size written in `RA2MD.INI` wins over the [resolution launch option](/using/command-line/resolution/), which fills in only a dimension the file leaves out.

If the renderer cannot start at the chosen size, the game shows a video error and exits.

[The sidebar](/systems/sidebar/) takes 168 pixels of the width, and the tactical view gets the rest.
