---
key: StockArtFixes
scope: client-settings
label: Fix stock art
when_omitted:
  kind: value
  value: "yes"
---

`StockArtFixes=yes` fills in art keys that the stock art file leaves out or comments out, so that stock art the original game draws wrongly looks as it was made to. Today it gives the apartment blocks `CITY01` to `CITY05` `Palette=city`, so they draw through the city palette of the map's theater (`CITYTEM.PAL`, `CITYSNO.PAL` or `CITYURB.PAL`). The original game has `;Palette=city` commented out in `ARTMD.INI`, so it draws their grey ramp through the house palette, which shows as speckled noise with solid patches on the roofs. `StockArtFixes=no` draws them as the original game does.

The key is read from the `[Video]` section of `RA2MD.INI`. The Graphics page has no switch for it, so it is set in the settings file only, and a change takes effect at the next start, when the art is read. A building type whose art section sets `Palette=`, in the stock file, a patch of it or a mod, keeps that value. The theaters that have no city palette file (new urban, lunar and desert) show the original look. Only drawing changes: the game logic and the art files are the same, and the game never writes to its data files.
