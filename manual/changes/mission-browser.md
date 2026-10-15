---
title: Browse every campaign mission in one screen
category: feature
release: 0.2.0
targets:
- type: key
  id: MenuStyle
  scope: client-settings
  effect: changed
credit:
- MentalHomiega
---

In the modern menus, New Campaign opens a mission browser in place of the campaign list and Mission Select. The browser lists every campaign mission in one panel, in groups by expansion and side: Yuri's Revenge Allied and Soviet, Red Alert 2 Allied and Soviet, other campaigns, and testing campaigns, which start closed. A group header opens or closes its group.

Selecting a mission shows its map picture and briefing. Play starts the selected mission, and a double click on a mission does the same. The difficulty drop-down sits at the foot of the panel, beside Back and Play.

A mission whose map file is missing stays listed, greyed out and marked "(cannot start)", and its reason shows under the picture. The browser selects the mission last chosen the next time it opens, which it keeps in `LastMission=` under `[Options]` in `RA2MD.INI`.

The classic menus keep the campaign list and Mission Select.
