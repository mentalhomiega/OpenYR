---
title: Hold 75 cameos in a sidebar strip by default and add LargeSidebarStrips for 225
category: fix
release: 0.2.0
targets:
- type: key
  id: LargeSidebarStrips
  effect: added
- type: system
  id: sidebar
  effect: changed
credit:
- MentalHomiega
---

A sidebar strip holds 75 cameos by default and refuses a further cameo while it is full. Before, a strip held 225. Yuri's Revenge allots 75 slots to each tab, and its test admits one cameo more than that, which writes one entry past the end of the array. `LargeSidebarStrips=yes` in `[General]` of `rules.ini` restores the 225-cameo strip for a rules set that needs it.

The save layout gained the setting, so a save written before this change no longer loads in this build.
