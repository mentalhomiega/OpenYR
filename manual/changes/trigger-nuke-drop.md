---
title: Drop the trigger nuke strike from above without crashing
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_MULTI_MISSILE
  effect: changed
- type: action
  id: TACTION_CHEM_MISSILE
  effect: changed
credit:
- MentalHomiega
---

The Nuke strike trigger action now drops the `NukePayload` weapon straight down onto its waypoint from 20000 leptons up, as in Yuri's Revenge. Before, it looked for a weapon named `MultiLauncher`, which the rules do not define, and the game crashed. Nuke strike and Chem-missile strike now fire nothing, with a line in the debug log, when their weapon is missing.
