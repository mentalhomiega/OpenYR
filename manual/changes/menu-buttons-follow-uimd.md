---
title: Follow the menu button actions of UIMD.INI
category: feature
release: 0.2.0
targets:
- type: format
  id: ui-ini
  effect: changed
- type: format
  id: mod-ini
  effect: changed
credit:
- MentalHomiega
---

The title menus follow the `SinglePlayerButton=`, `NetworkButton=`, `MoviesAndCreditsButton=`, `CampaignButton=`, `SkirmishButton=`, `SneakPeeksButton=`, `PlayMoviesButton=` and `ViewCreditsButton=` keys of `[UISettings]` in the UI file, which hide, disable or redirect a button as Ares does. A mod whose UI file hides buttons for a launcher it is played through can keep every button with `FollowMenuButtons=no` under `[Menu]` in its `mod.ini`.
