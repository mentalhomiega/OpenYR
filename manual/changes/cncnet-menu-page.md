---
title: Set up and start a CnCNet copy from the main menu
category: feature
release: 0.2.0
targets:
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

The main menu has a CnCNet entry in both menu styles. In the classic style it replaces the greyed Internet button, and it sits after Single Player. The page finds a CnCNet Yuri's Revenge install, or uses the one it remembers, and makes a copy that starts OpenYR's `Game.exe`, as the `cncnet_tester_setup.ps1` script does. The copy goes beside the install, as `<install> - OpenYR`, or to `OpenYR CnCNet` in a `Games` folder on the Windows drive when the install's parent folder cannot be written. Change destination... picks another folder. The page remembers the install and the copy folder in the settings file.

The page lists every change before it makes one. It refuses a destination that cannot be written, a non-empty folder that is not an OpenYR copy, and a drive without the free space the install needs. It reports whether the copy and its edits are in place, and can update, repair, remove or start the copy. A copy runs in the background, shows its progress and can be cancelled. The install is only read.
