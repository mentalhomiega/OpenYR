---
title: Show the Yuri's Revenge campaign loading screens
category: fix
release: 0.2.0
targets:
- type: format
  id: missionmd-ini
  effect: added
- type: system
  id: loading-screens
  effect: changed
- type: key
  id: CD
  scope: campaign
  effect: changed
credit:
- MentalHomiega
---

A campaign mission now loads behind the picture, title and objectives that `MISSIONMD.INI` names for it, between Yuri's Revenge's title bar and progress strip, instead of a Tiberian Sun picture with its loading messages. On a large screen the layout is enlarged by a whole number, three times on a 3840 by 2160 screen. A mission without a picture in `MISSIONMD.INI` still shows the older screen.
