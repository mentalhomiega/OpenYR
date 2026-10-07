---
title: Loading screens
summary: "Shows a picture, the mission's or the country's text, and progress bars while a scenario loads."
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

The game looks for the screen's files as for any other, and also inside `LOADMD.MIX` and `LOAD.MIX`, which it mounts only while the screen is up. Text is printed in the game font. A string table label the table does not have shows as `MISSING:'<label>'`.

## A campaign mission

A campaign mission shows the picture [MISSIONMD.INI](/formats/missionmd-ini/) names for it, laid out on 800 by 600 pixels:

| Rows | Art | Palette |
| --- | --- | --- |
| The top 40 | Title bar, `TTLBR800.SHP` | `LDSCRN.PAL` |
| The next 520 | The mission's picture, `LS800BkgdName=` | `LS800BkgdPal=` |
| The bottom 40 | Progress strip, `SPLDBRL.SHP` | `SPLDBR.PAL` |

On a screen 640 pixels wide the layout is 640 by 480 instead. Its title bar is `TTLBR640.SHP`, its picture is `LS640BkgdName=` in 400 rows, still drawn with the `LS800BkgdPal=` palette, and its progress strip is `SPLDBRS.SHP`. Missing title bar or strip art leaves those rows black.

The title bar shows the text `LSLoadMessage=` names, 10 pixels in from the bar's left and top edges. The text `LSLoadBriefing=` names is printed over the picture, with its top-left corner `LS800BriefLocX=` pixels right of and `LS800BriefLocY=` pixels below the picture's top-left corner; the 640 by 480 layout uses `LS640BriefLocX=` and `LS640BriefLocY=`. The briefing breaks onto a new line at each line break in the string, and at the space that keeps a line within 400 pixels. The picture behind the briefing is darkened to about two-fifths of its brightness, in a box reaching 4 pixels beyond the text on every side.

Both texts are printed in the `AlliedLoad` color of the rules `[Colors]` section for a campaign whose [`CD=`](/keys/cd/) is `0`, and in `SovietLoad` for any other value. The stock campaigns all have `CD=2`, so their text is red. A mission launched without a campaign takes the campaign that lists that mission as its `Scenario=`, and uses `AlliedLoad` when no campaign lists it.

The progress bar is the first frame of `SPLDBR.SHP`, drawn with `SPLDBR.PAL` in the progress strip, starting 172 pixels from its left edge in the 800 by 600 layout and 92 pixels in the 640 by 480 one. It shows as much of its width as the share of the scenario already loaded.

A mission falls back to the older loading screen, which shows a picture picked for the campaign's side, when MISSIONMD.INI is missing, has no section for it, names no picture for the screen's layout, or names a picture or palette the game cannot find or read. [`CD=`](/keys/cd/) describes how that picture is picked.

## Skirmish and multiplayer

A skirmish or multiplayer match shows a picture for the local player's country, laid out on 800 by 600 pixels, or on 640 by 480 on a screen 640 pixels wide. The country is picked by its place in the rules `[Countries]` list, counted from `0`, not by its name, so reordering the list changes the art a country gets:

| Place | Stock country | Picture | Palette | Flag |
| --- | --- | --- | --- | --- |
| 0 | Americans | `LS800USTATES.SHP` | `MPLSU.PAL` | `USAI.PCX` |
| 1 | Alliance | `LS800KOREA.SHP` | `MPLSK.PAL` | `JAPI.PCX` |
| 2 | French | `LS800FRANCE.SHP` | `MPLSF.PAL` | `FRAI.PCX` |
| 3 | Germans | `LS800GERMANY.SHP` | `MPLSG.PAL` | `GERI.PCX` |
| 4 | British | `LS800UKINGDOM.SHP` | `MPLSUK.PAL` | `GBRI.PCX` |
| 5 | Africans | `LS800LIBYA.SHP` | `MPLSL.PAL` | `DJBI.PCX` |
| 6 | Arabs | `LS800IRAQ.SHP` | `MPLSI.PAL` | `ARBI.PCX` |
| 7 | Confederation | `LS800CUBA.SHP` | `MPLSC.PAL` | `LATI.PCX` |
| 8 | Russians | `LS800RUSSIA.SHP` | `MPLSR.PAL` | `RUSI.PCX` |
| 9 | YuriCountry | `LS800YURI.SHP` | `MPYLS.PAL` | `YRII.PCX` |
| An observer | | `LS800OBS.SHP` | `MPLSOBS.PAL` | `OBSI.PCX` |

The 640 by 480 layout uses the pictures named `LS640` instead of `LS800`. A player whose country is past place 9, or whose picture or palette the game cannot find or read, gets the older loading screen, with a picture picked for the player's side.

The screen prints three texts from the string table over the picture, taken from labels fixed for each place:

| Place | Country name | Special unit | Description |
| --- | --- | --- | --- |
| 0 | `Name:Americans` | `Name:Para` | `LoadBrief:USA` |
| 1 | `Name:Alliance` | `Name:BEAGLE` | `LoadBrief:Korea` |
| 2 | `Name:French` | `Name:GTGCAN` | `LoadBrief:French` |
| 3 | `Name:Germans` | `Name:TNKD` | `LoadBrief:Germans` |
| 4 | `Name:British` | `Name:SNIPE` | `LoadBrief:British` |
| 5 | `Name:Africans` | `Name:DTRUCK` | `LoadBrief:Lybia` |
| 6 | `Name:Arabs` | `Name:DESO` | `LoadBrief:Iraq` |
| 7 | `Name:Confederation` | `Name:TERROR` | `LoadBrief:Cuba` |
| 8 | `Name:Russians` | `Name:TTNK` | `LoadBrief:Russia` |
| 9 | `Name:YuriCountry` | `Name:YURI` | `LoadBrief:YuriCountry` |
| An observer | `Name:Observer` | | |

The country name and the description are printed in `AlliedLoad` for a country of the first side in the rules `[Sides]` list, in `SovietLoad` for any other side, and in `LightGrey` for an observer. The special unit's name is printed in black capitals. The text of `GUI:LoadingEx` is printed in the country name's color. The picture is darkened behind the country name, the description and `GUI:LoadingEx`, as behind a campaign briefing, and the description's box is as wide as its wrapping width.

| Item | 800 by 600 | 640 by 480 |
| --- | --- | --- |
| Country name, right-aligned | Right edge at x 740, y 310 | Right edge at x 585, y 436 |
| Special unit's name | x 20, y 90 | x 16, y 72 |
| Description, wrapped at | x 20, y 158, 398 pixels | x 16, y 126, 318 pixels |
| `GUI:LoadingEx` | x 20, y 300 | x 16, y 235 |
| Map preview box | 216 by 166 at x 499, y 379 | 200 by 200 at x 385, y 270 |
| First player row | x 16, y 321 | x 12, y 256 |

The map preview is the map's `[Preview]` picture, enlarged or reduced to fit the preview box without changing its proportions, centered on a black box. Once the houses have their start positions, the preview's own marks on the first start positions are blacked out, and `MMPB.SHP` marks each house's start position in the house's color. A house that starts away from the numbered start positions gets no mark. A random map, or a map without a `[Preview]` section, shows no preview.

A multiplayer match has a row for each player, and a skirmish one for the local player. From left to right, a row shows a bar in the player's color, the flag of the player's country, and the player's name in the player's color. The bar is the first frame of `PROGBARM.SHP` inside an outline in the player's color, and shows as much of its width as the share of the scenario that player has loaded. Rows follow each other down the screen, each as tall as the tallest of the bar's outline, the `USAI.PCX` flag and a line of text, plus 4 pixels. Magenta pixels of a flag are left out.

## Size on large screens

The layout is drawn in the middle of the screen, enlarged smoothly, every pixel blended with its neighbors, to the largest size that fits at its own proportions: the screen's full height on a screen at least as wide as the layout's 4 to 3. A 1920 by 1080 screen therefore shows an 800 by 600 layout at 1.8 times its size, 1440 by 1080 pixels, a 2560 by 1440 screen at 2.4 times, and a 3840 by 2160 screen at 3.6 times. Black fills the rest of the screen. The text is part of the picture and is enlarged with it.
