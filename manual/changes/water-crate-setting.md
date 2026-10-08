---
title: Read WaterCrate and WaterCrateImg for campaign crates
category: feature
release: 0.2.0
targets:
- type: key
  id: WaterCrate
  effect: added
- type: key
  id: WaterCrateImg
  effect: added
credit: [MentalHomiega]
---

We now read `WaterCrate=` and `WaterCrateImg=` from `[CrateRules]`. In a campaign, a collected crate on the `WaterCrateImg` overlay gives the `WaterCrate` result, as in Yuri's Revenge. Before, we ignored both keys, so such a crate always gave money. The default and the shipped rules both say `Money`, so existing rules play the same. These values are part of the save, so saved games from earlier builds no longer load.
