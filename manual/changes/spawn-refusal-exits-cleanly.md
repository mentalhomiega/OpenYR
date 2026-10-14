---
title: Exit cleanly when a network launch is refused
category: fix
release: 0.2.0
targets:
- type: format
  id: spawn-ini
  effect: changed
credit:
- MentalHomiega
---

A launch refused after the network failed to open, such as when its UDP port is already in use, crashed the game. The refusal's message box ran the game loop before any match existed, and the sidebar read the missing local house. The box now runs only the maintenance callbacks while no local house exists, the sidebar checks for a local house, and a launch with `-AUTOTEST` logs the reason without the box. The game then exits as it does for any other refused launch.
