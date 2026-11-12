---
key: PerfOverlay
scope: client-settings
label: Performance overlay
when_omitted:
  kind: value
  value: "Off"
---

`PerfOverlay=` in the `[Video]` section of `RA2MD.INI` shows the performance overlay over the top left of the map view during a game. `Basic` shows the game and drawn frame rates with their frame times and a graph of the recent frame times of each. `Detailed` also lists the time of each part of a game frame, a draw and a present, a few counts, and the window size, zoom, renderer and drawing settings. `Off` hides it. Another value reads as `Off`.

The [`PerfOverlay`](/commands/perfoverlay/) command, Ctrl+Shift+P by default, steps through the levels during a game and changes this setting, so the next save of the settings keeps the level it left. The Graphics page shows the setting as the Performance overlay switch, which picks `Basic`.

To report a slow game, set `PerfOverlay=Detailed`, play until the game slows, and take a screenshot that shows the overlay.
