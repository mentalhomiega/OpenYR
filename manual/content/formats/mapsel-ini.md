---
format_id: mapsel-ini
title: MAPSELMD.INI
summary: Lists each campaign side's stages in order, and the mission each stage plays and the first choice it leads to.
kind: file
filenames:
  - MAPSELMD.INI
source_files:
  - code/mschoice.cpp
  - code/mapsel.cpp
related:
  - type: system
    id: campaign-progression
  - type: format
    id: vqa
  - type: format
    id: aud
---

When a campaign mission is won and the campaign continues, the next mission is a choice that this file lists for the mission's stage. A mission that sets [`SkipMapSelect=yes`](/keys/skipmapselect/) names that choice itself. [Campaign progression and carry-over](/systems/campaign-progression/) covers that sequence and the state carried across it.

## Which file is read

Every campaign mission reads `MAPSELMD.INI`, whichever expansion it belongs to. The game reads the section named after the side of the player's house. The side comes from the house's `Side=` key in the rules, so a GDI house such as Americans reads `[GDI]`, and a Nod house such as Russians reads `[Nod]`. A house on no side, or on a side with no section of its own, such as the third side, fails the read. So does a missing file, or a side section with no numbered entries. [The win sequence](/systems/campaign-progression/#the-win-sequence) covers what happens then.

The file is read again at every advance, so an edit takes effect at the next advance.

## Side sections

A side section lists the stages of that side's campaign in order. Each numbered key from `1` upward names a stage section. The list ends at the first missing number, so number the entries from `1` without gaps. At most 100 stages are read.

A stage's position in the list is its stage number. The campaign keeps the number of the stage the player has reached, and an advance with [`SkipMapSelect=yes`](/keys/skipmapselect/) finds the next stage by that number. Inserting, removing or reordering entries changes the stage numbers, so a campaign saved before the edit continues from whichever stage now holds its number. A win without `SkipMapSelect=yes` does not use the number either. It starts at the stage whose `Scenario=` is the mission just won.

Two further keys name sections that hold decoration for the map selection screen, which does not open:

- `Anims=` names the section of animations and the screen's palette.
- `Sounds=` names the section of sound effects.

## Stage sections

Each stage has a section whose name is the label the side list uses. Its `Scenario=` key names the mission the stage plays.

| Key | Value |
| --- | --- |
| `Scenario=` | The mission file the stage plays. [`NextScenario=`](/keys/nextscenario/) must match this value to reach the stage. |
| `Description=` | Text the map selection screen would print for a region that leads to the stage. A number selects an entry of the game's string table. A value starting with a letter names a section of this file; every entry of that section, in order, is joined with single spaces into one description of up to 1,023 characters. |
| `VoiceOver=` | An [AUD](/formats/aud/) file the map selection screen would stream for a region that leads to the stage. |

`Description=` and `VoiceOver=` belong to the map selection screen, which the game does not draw. The game reads them and they do not change which mission loads.

The other keys belong to the map selection screen as well. Only `MapVQ=` affects play. The game reads the rest and draws none of them.

| Key | Value |
| --- | --- |
| `MapVQ=` | The [VQA](/formats/vqa/) movie the screen would open with. A stage with a value takes its first choice after a win. A stage without one loads the mission just won again. The movie is not played. [When the choice fails](/systems/campaign-progression/#when-the-choice-fails) says what loads next in each case. |
| `Overlays=` | Up to two comma-separated shape file names, faded in one after the other over the movie's last frame. A third name is ignored. |
| `ClickMap=` | A 256-color PCX file whose pixel colors would mark out the selectable regions. Its top-left pixel is the top-left corner of the 640 by 400 presentation area. |
| `Targets=` | `<count>,<x>,<y>,<x>,<y>...`: the number of target markers, then the center of each in the 640 by 400 presentation area. The first marker belongs to the choice with the lowest key number, the second to the next, and so on. Give no more markers than the stage has choices. |
| `Text1=` to `Text7=` | Captions for the movie, each `<x>,<y>,<delay>,<text>`. The delay counts 16-millisecond ticks from the start of the movie, about 60 to the second. The text ends at its first comma. |

The numbered keys of a stage section are its choices. Each key is a click-map pixel color from `0` to `255`, and its value is the label of the stage that color selects on the screen. After a win, the choice with the lowest key number is the first choice. The key numbers need not start at `1` or run without gaps. Labels are matched without regard to case.

A choice whose label names no stage of the side cannot be reached. If it is the first choice, the mission just won loads again.

This excerpt shows a side list and two stage sections, and the two shapes a choice list takes:

```ini title="MAPSELMD.INI"
[GDI]
Anims=Anims
Sounds=GDISFX
1=GDI01   ;1A
2=GDI02   ;2A
3=GDI03   ;3A1
4=GDI04   ;3A2

[GDI01]
Scenario=GDI1A.MAP
Description=768
VoiceOver=GDI-01.AUD
MapVQ=GDIMAP01.VQA
Overlays=RG02A.SHP,RN02A.SHP
Targets=1,180,80
ClickMap=GDICLK01.PCX
2=GDI02 ;2A

[GDI02]
Scenario=GDI2A.MAP
Description=769
VoiceOver=GDI-02.AUD
MapVQ=GDIMAP01.VQA
Overlays=RG03AB.SHP,RN03AB.SHP
Targets=2,290,88,218,108
ClickMap=GDICLK01.PCX
3=GDI04 ;3A2
4=GDI05 ;3B
```

`[GDI]` lists stages 1 to 4 in the order written. Winning `GDI1A.MAP`, the mission of stage `[GDI01]`, has one choice: color 2 leads to `[GDI02]`. Winning `GDI2A.MAP` has two. The first choice, color 3, leads to `[GDI04]`, so a win takes it. Color 4 leads to `[GDI05]` and is reached only through `SkipMapSelect=yes`, when the mission's `NextScenario=` names the mission of `[GDI05]`. For that to work, `[GDI]` must also list `GDI05`, which this excerpt leaves out. The excerpt also leaves out the animation and sound sections the side section names; both are described below.

## How a stage is chosen

Both ways of advancing end at a stage of this file, never directly at a mission file.

After a win without `SkipMapSelect=yes`, the stage whose `Scenario=` is the mission just won is found:

- If the stage has no `MapVQ=`, the mission just won loads again.
- If the stage has a `MapVQ=`, the `LOADING` track plays when the game has it, and the first choice is taken. The stage that choice leads to supplies the mission that loads.

If the house has no side, if `MAPSELMD.INI` cannot be read or lists no stages for the side, or if no stage of the side has the mission just won as its `Scenario=`, the game shows an error box, and the mission just won loads again. A first choice whose stage has no `Scenario=`, or whose label names no stage of the side, loads the mission just won again without an error box.

With `SkipMapSelect=yes`, the name given by [`NextScenario=`](/keys/nextscenario/) or [`AltNextScenario=`](/keys/altnextscenario/) is compared, ignoring case, with the `Scenario=` value of each stage the current stage offers. The stages are tried in order of their key numbers, and the first match is taken. Only stages that the current stage offers can be reached this way.

## Animation and sound sections

No screen draws or plays anything from the sections that `Anims=` and `Sounds=` name, so an edit to them does not change which mission loads. Their formats are below.

The section named by `Anims=` describes the screen's text area, its palette and its looping animations:

- `TextRect=` is `<x>,<y>,<width>,<height>`, the area stage descriptions would be printed in, relative to the 640 by 400 presentation area. If it is left out, descriptions use the whole area.
- `Palette=` names the palette the animations and target markers would be drawn with.
- The numbered keys from `1` upward each hold `<file>,<x>,<y>,<delay>`: a shape file, its position in the presentation area, and the delay between its frames in 16-millisecond ticks. The list ends at the first missing number, and at most 100 are read.

The section named by `Sounds=` assigns a sample to each sound event of the screen. Each key is an event name, and each value is `<file>,<volume>`. The volume is a percentage, limited to the range `0` to `100`. Without a volume the sample plays at full volume, and a volume of `0` leaves the event silent. The events are:

| Event | Plays when |
| --- | --- |
| `Overlay` | Each overlay appears. |
| `TargetFlyIn` | Each target marker appears. |
| `EnterRegion` | The cursor enters a region that has a choice. |
| `ExitRegion` | The cursor leaves a region that has a choice. |
| `MouseOnMap` | The cursor moves from color `0` of the click map onto any other color. |
| `MouseOffMap` | The cursor moves back onto color `0`. |
| `Click` | The player clicks a region that has a choice. |

An event the section does not set is silent.
