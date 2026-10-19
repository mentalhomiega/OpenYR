---
title: Pick the mod from the launcher
category: feature
release: 0.2.0
targets:
- type: key
  id: Mods
  effect: changed
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

`OpenYR.exe` now opens a window before it starts the game, listing Yuri's Revenge without mods and every folder in the `Mods` folder with its name and description. Play saves the pick as `Mods=` in `RA2MD.INI`, where the Mods page reads and writes it too. A list the picker cannot show as one choice, such as several mods, appears as a first row that keeps it. **Do not ask again**, or the new check box on the Mods page, sets `ModPicker=` under `[Launcher]` in `RA2MD.INI`. The Mods page also says that a changed list takes effect after a restart as soon as the list changes.
