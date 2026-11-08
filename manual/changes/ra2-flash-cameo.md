---
title: Flash the sidebar cameos that Red Alert 2 missions number
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_FLASH_CAMEO
  effect: changed
credit:
- MentalHomiega
---

A Red Alert 2 mission that flashes a cameo in the sidebar now flashes the right one. Red Alert 2 names the object type by its place in one list of every type (buildings first, then vehicles, aircraft and infantry), where Yuri's Revenge writes the type's ID, so before this the number matched no type and nothing flashed. The Boot Camp tutorials now flash the power plant, refinery, barracks and GI cameos as the drill sergeant names them. The list is read from the rules file `RA2Rules=` names.
