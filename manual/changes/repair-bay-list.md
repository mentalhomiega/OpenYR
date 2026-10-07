---
title: Read RepairBay as a list of buildings
category: fix
release: 0.2.0
targets:
- type: key
  id: RepairBay
  effect: changed
credit:
- MentalHomiega
---

`RepairBay` in `[General]` is read as a list of building types, as in Yuri's Revenge, and the first one that exists is used. The list was read as a single name, which made a new building type called `GADEPT,NADEPT,CAOUTP` after all the others. Every building type a map added then had a number one too high, so the campaign triggers that count or test a building by its number, such as the power plant checks in Allied mission 1, looked at the wrong buildings.
