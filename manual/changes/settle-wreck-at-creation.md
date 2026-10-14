---
title: Settle a wreck's passengers, crew and crate when it is created
category: feature
release: 0.2.0
targets:
- type: key
  id: DeathFrames
  effect: changed
- type: key
  id: CrewEscape
  effect: changed
- type: key
  id: CarriesCrate
  effect: changed
- type: system
  id: destruction-and-debris
  effect: changed
credit:
- MentalHomiega
---

A vehicle with [`DeathFrames`](/keys/deathframes/) now settles its passengers, crew and truck crate when it becomes a wreck, as Yuri's Revenge does. Before, a wreck settled none of them, and its passengers died when it was removed. The crew roll and a hijacker's step out come from [`CrewEscape`](/keys/crewescape/), and the crate comes from [`CarriesCrate`](/keys/carriescrate/) under the scenario's crate setting. The passengers follow [`WreckReleasesPassengers`](/keys/wreckreleasespassengers/#scope-unittype), so by default they get out of a wreck. Each of these settles once, when the wreck is created. A later hit that destroys the wreck again does not repeat them.
