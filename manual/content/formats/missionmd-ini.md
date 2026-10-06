---
format_id: missionmd-ini
title: MISSIONMD.INI
summary: Names each campaign mission's written briefing and the picture and text of its loading screen.
kind: file
filenames:
  - MISSIONMD.INI
source_files:
  - code/loadscreen.cpp
  - code/restate.cpp
related:
  - type: system
    id: loading-screens
  - type: key
    id: Brief
  - type: format
    id: csf
  - type: format
    id: shp
---

Each campaign mission has a section named after the map file the mission is started with, such as `[ALL01UMD.MAP]`. The game reads the file each time a campaign mission loads and each time the objectives screen opens, so an edit applies the next time either happens.

A value that names text is a label of the game's [string table](/formats/csf/). A label the table does not have shows as `MISSING:'<label>'`.

| Key | Value |
| --- | --- |
| `Briefing=` | The label of the written briefing on the objectives screen. [`Brief`](/keys/brief/) covers that screen and what it shows without this key. |
| `LSLoadMessage=` | The label of the line printed in the loading screen's title bar. Omitted, the title bar has no text. |
| `LSLoadBriefing=` | The label of the text printed over the loading picture. Omitted, the picture has no text. |
| `LS800BkgdName=` | The loading picture, a [SHP](/formats/shp/) file. Write the file name with its extension. |
| `LS640BkgdName=` | The loading picture on a screen 640 pixels wide. |
| `LS800BkgdPal=` | The palette file both pictures are drawn with. |
| `LS800BriefLocX=`, `LS800BriefLocY=` | Where the loading briefing's top-left corner sits, in pixels right of and below the picture's top-left corner. Each is `0` when omitted. |
| `LS640BriefLocX=`, `LS640BriefLocY=` | The same position for the picture on a screen 640 pixels wide. |

[Loading screens](/systems/loading-screens/#a-campaign-mission) covers where the picture and the texts appear, their color, and the older screen a mission shows when its picture or palette is missing.

`UIName=` is not read.

```ini title="MISSIONMD.INI"
[ALL01UMD.MAP]
Briefing=Brief:All01md
LSLoadMessage=LoadMsg:All01md
LSLoadBriefing=LoadBrief:All01md
LS640BriefLocX=20
LS640BriefLocY=20
LS800BriefLocX=20
LS800BriefLocY=20
LS640BkgdName=LS640A01.SHP
LS800BkgdName=LS800A01.SHP
LS800BkgdPal=LS800A01.PAL
```
