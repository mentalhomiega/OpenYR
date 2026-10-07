---
key: InterfaceScale
scope: client-settings
label: Interface scale
see_also: [ScreenWidth, ScreenHeight, CursorScale, ScaleMode]
when_omitted:
  kind: value
  value: Auto
  note: The scale is the screen height divided by 1080, rounded to a whole number, so it is 1 up to 1619 lines high and 2 from 1620 to 2699.
---

`InterfaceScale=` sets how many times larger than its original size the in-game interface is drawn: the sidebar and its cameos, the radar, the top tabs, the command bar, tooltips, messages, and the dialogs the game draws itself. It is written in `[Video]`. The value is `Auto` or a number.

At `Auto`, or with the key missing, the scale is [`ScreenHeight`](/keys/screenheight/) divided by 1080 and rounded to the nearest whole number. A 1920 by 1080 screen gets 1, so nothing changes there, and a 3840 by 2160 screen gets 2, which makes the interface the size it is at 1920 by 1080. A number sets the scale directly, and may have a fraction: `1.5` on a 3840 by 2160 screen gives the interface in a 2560 by 1440 area. A value below 1 acts as 1, a value above 8 acts as 8, and the scale is never so large that the interface area would fall below 640 by 480.

[`ScreenWidth`](/keys/screenwidth/) and `ScreenHeight` are the size of the window, or in fullscreen the size of the display. The game divides them by the scale to get the size of the area it draws the interface in, and enlarges that area to fill the window as [`ScaleMode`](/keys/scalemode/) says. The map is not drawn in that area. It is drawn at the screen's own resolution and shown under the interface, so a 3840 by 2160 screen shows the map in full detail beside a sidebar that is twice the size of the original. Clicks and the mouse pointer follow: a click on a cameo or a button lands where the enlarged picture shows it, and the pointer is enlarged by [`CursorScale`](/keys/cursorscale/), which at `0` follows the same enlargement.

The map's zoom is measured from the screen's own pixels: at a zoom of 1, one map pixel is one screen pixel, whatever the scale. The mouse wheel zooms from 0.5 to 2 as before, and a multiplayer game zooms out no further than 0.75. A game starts at a zoom of 1, and picking a new screen size on the display options screen works out the scale again and returns the zoom to 1.

Screenshots taken with the screenshot command are the size of the interface area, not of the screen, with the map scaled down into them.

The setting is read when the game starts, and the display options screen writes `Auto` or the number back to `RA2MD.INI` with the other video settings.
