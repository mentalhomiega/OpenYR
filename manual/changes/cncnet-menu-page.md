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

The main menu has a CnCNet entry in both menu styles. In the classic style it replaces the greyed Internet button, and it sits after Single Player. The page finds a CnCNet Yuri's Revenge install, or asks for its folder, and makes a copy that starts OpenYR's `Game.exe`, as the `cncnet_tester_setup.ps1` script does. The page lists every change before it makes one, reports whether the copy and its edits are in place, and can update, repair, remove or start the copy. A copy runs in the background, shows its progress and can be cancelled. The install is only read.
