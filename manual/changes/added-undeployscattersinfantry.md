---
title: Let allied soldiers step off an undeploy cell
category: feature
release: 0.2.0
targets:
- type: key
  id: UndeployScattersInfantry
  effect: added
credit:
- MentalHomiega
---

A structure that is sold into its vehicle can now wait for allied soldiers to leave the cell the vehicle appears on. `UndeployScattersInfantry=yes` makes those soldiers step aside and gives them up to five seconds, so a deployed Slave Miner sold with a slave on that cell keeps its slaves. The default `no` keeps the original behavior, where the vehicle fails to appear and the slaves are freed. See [`UndeployScattersInfantry`](/keys/undeployscattersinfantry/) for the exact rules.
