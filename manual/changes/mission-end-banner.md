---
title: Show Yuri's Revenge's mission end banners
category: fix
release: 0.2.0
targets:
- type: system
  id: eva-speech
  effect: changed
credit:
- MentalHomiega
---

Winning or losing a campaign mission, and winning or losing a skirmish or network match, now puts up the banner Yuri's Revenge shows. Before, a line of white text from the old Tiberian Sun string table was drawn across the middle of the view. The banner is the picture from `GRFXTXT.SHP`: Mission Accomplished, Mission Failed, You Are Victorious! or Game Over, each its own frame, centred in the view and drawn in the palette `GRFXTXT.PAL`. The voice line is the same as before.

A game without `GRFXTXT.SHP` or `GRFXTXT.PAL` draws the banner's wording as plain white text instead. The wording is read from the `TXT_SCENARIO_WON`, `TXT_SCENARIO_LOST`, `TXT_VICTORIOUS` and `TXT_LOST` labels of the string table, so a mod's string table changes it.

Saves made by earlier builds no longer load.
