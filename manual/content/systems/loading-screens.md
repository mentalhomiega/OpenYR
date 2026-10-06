---
title: Loading screens
summary: "Covers the screen while a scenario loads with a picture, the mission's text and a progress bar."
category: rendering-presentation
keys: []
related:
  - type: format
    id: missionmd-ini
  - type: format
    id: spawn-ini
  - type: key
    id: CD
    scope: campaign
---

A loading screen covers the game from the start of a mission, match or restart until its map is ready. Loading a saved game shows none.

A launch file's [`CustomLoadScreen`](/formats/spawn-ini/#what-a-player-is-shown) replaces every loading screen this page describes, as long as the named picture is found.

## A campaign mission

A campaign mission shows the picture [MISSIONMD.INI](/formats/missionmd-ini/) names for it, laid out on 800 by 600 pixels:

| Rows | Art | Palette |
| --- | --- | --- |
| The top 40 | Title bar, `TTLBR800.SHP` | `LDSCRN.PAL` |
| The next 520 | The mission's picture, `LS800BkgdName=` | `LS800BkgdPal=` |
| The bottom 40 | Progress strip, `SPLDBRL.SHP` | `SPLDBR.PAL` |

On a screen 640 pixels wide the layout is 640 by 480 instead. Its title bar is `TTLBR640.SHP`, its picture is `LS640BkgdName=` in 400 rows, still drawn with the `LS800BkgdPal=` palette, and its progress strip is `SPLDBRS.SHP`. Missing title bar or strip art leaves those rows black.

The game looks for these files as for any other, and also inside `LOADMD.MIX` and `LOAD.MIX`, which it mounts only while the screen is up.

The title bar shows the text `LSLoadMessage=` names, 10 pixels in from the bar's left and top edges. The text `LSLoadBriefing=` names is printed over the picture, with its top-left corner `LS800BriefLocX=` pixels right of and `LS800BriefLocY=` pixels below the picture's top-left corner; the 640 by 480 layout uses `LS640BriefLocX=` and `LS640BriefLocY=`. The briefing breaks onto a new line at each line break in the string, and at the space that keeps a line within 400 pixels. The picture behind the briefing is darkened to about two-fifths of its brightness, in a box reaching 4 pixels beyond the text on every side.

Both texts are printed in the game font, in the `AlliedLoad` color of the rules `[Colors]` section for a campaign whose [`CD=`](/keys/cd/) is `0` and in `SovietLoad` for any other value. The stock campaigns all have `CD=2`, so their text is red. A mission started outside a campaign uses `AlliedLoad`.

The progress bar is the first frame of `SPLDBR.SHP`, drawn with `SPLDBR.PAL` in the progress strip, starting 172 pixels from its left edge in the 800 by 600 layout and 92 pixels in the 640 by 480 one. It shows as much of its width as the share of the scenario already loaded.

A mission falls back to the older loading screen, which shows a picture picked for the campaign's side, when MISSIONMD.INI is missing, has no section for it, names no picture for the screen's layout, or names a picture or palette the game cannot find or read. [`CD=`](/keys/cd/) describes how that picture is picked.

## Skirmish and multiplayer

A skirmish or multiplayer match shows the older loading screen, with a picture picked for the player's side.

## Size on large screens

The layout is drawn in the middle of the screen, every pixel enlarged to a square of the largest whole number of pixels that lets the layout fit. A 1920 by 1080 screen therefore shows it at its own size, and a 3840 by 2160 screen at three times its size, 2400 by 1800 pixels. Black fills the rest of the screen.
