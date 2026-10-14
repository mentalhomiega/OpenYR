---
title: Open the mission briefing from the classic sidebar's diplomacy button
category: fix
release: 0.2.0
targets:
- type: system
  id: sidebar
  effect: changed
credit:
- MentalHomiega
---

In the classic sidebar, the button on the left above the radar opens the mission briefing in a campaign, as Yuri's Revenge's does. The dialog shows the text of the options menu's Restate Briefing without its movie choice. The button's tooltip reads Briefing in a campaign and Diplomacy elsewhere. In skirmish and multiplayer the button still does nothing, because OpenTS has no diplomacy screen.

Also in the classic sidebar, a tab with no items is disabled and cannot be clicked. The options and diplomacy buttons now show their tooltips. The down arrow sits on the left and the up arrow on the right, as in Yuri's Revenge.
