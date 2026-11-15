---
title: Give each side its own sidebar archives, radar logo and announcer
category: feature
release: 0.2.0
targets:
- type: key
  id: Sidebar.MixFileIndex
  effect: added
- type: key
  id: Sidebar.YuriFileNames
  effect: added
- type: key
  id: ToolTipColor
  effect: added
- type: key
  id: MessageTextColor
  effect: added
- type: key
  id: EVA.Tag
  effect: added
- type: format
  id: eva-ini
  effect: changed
- type: system
  id: sidebar
  effect: changed
- type: system
  id: eva-speech
  effect: changed
credit:
- MentalHomiega
---

A side's section can name the sidebar archives it mounts with `Sidebar.MixFileIndex=` and choose the radar logo files with `Sidebar.YuriFileNames=`, so a mod with more than three sides gives each its own sidebar art in the classic and the modern interface. `EVA.Tag=` picks the announcer voice, and `[EVATypes]` in the EVA file declares voices beyond Allied, Russian and Yuri. `ToolTipColor=` colors the classic tooltips and `MessageTextColor=` the text a map's text action prints. The key names follow Ares. A side that sets none of these behaves as before, except that the third side now draws its radar logo from `RADARY.SHP` and `RADARYURI.PAL`, as Yuri's Revenge does.
