---
title: Read harvester search distances from General
category: feature
release: 0.2.0
targets:
- type: key
  id: TiberiumNearScan
  effect: removed
- type: key
  id: TiberiumFarScan
  effect: removed
- type: key
  id: TiberiumShortScan
  effect: added
- type: key
  id: TiberiumLongScan
  effect: added
credit: [MentalHomiega]
---

Harvester search distances now come from `TiberiumShortScan` and `TiberiumLongScan` in `[General]`, as in Yuri's Revenge. `[AI]` `TiberiumNearScan=` and `TiberiumFarScan=` are no longer read; move them to the new names.
