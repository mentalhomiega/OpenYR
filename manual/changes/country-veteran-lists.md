---
title: Make a country's listed types veterans when built
category: feature
release: 0.2.0
targets:
- type: key
  id: VeteranInfantry
  effect: added
- type: key
  id: VeteranUnits
  effect: added
- type: key
  id: VeteranAircraft
  effect: added
credit: [MentalHomiega]
---

We now read `VeteranInfantry=`, `VeteranUnits=` and `VeteranAircraft=` from a country's section. A new object of a listed type, made for a house of that country, starts as a veteran, as in Yuri's Revenge. Before, we ignored these keys, so a new object of a listed type stayed a rookie unless a spy or another promotion made it a veteran. The lists are now part of the save, so saved games from earlier builds no longer load.
