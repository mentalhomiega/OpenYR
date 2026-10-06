---
title: Number objects made after a load as the saved game would have
category: fix
release: 0.2.0
targets:
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

Objects created after a load now get the identifiers they would have had in the saved game. Each object rebuilt by the load used to take a fresh identifier, so later missiles weaved along different paths and particles advanced on different frames, and a loaded game could play out differently from the saved one.
