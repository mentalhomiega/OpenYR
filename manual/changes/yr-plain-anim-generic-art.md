---
title: Draw plain animations from the generic art when their own file is missing
category: fix
release: 0.2.0
targets:
- type: key
  id: Image
  scope: animtype
  effect: changed
credit:
- MentalHomiega
---

An animation whose `<Image ID>.SHP` is missing now draws the generic file of the same name, with `G` as its second letter, as Yuri's Revenge does. This applies to animations that are neither `NewTheater=yes` nor `DemandLoad=yes`. The Yuri Grinder's rollers now show while it is idle (`YAGRND_A`) and while it grinds a unit (`YAGRND_B`, drawn from `YGGRND_A.SHP` and `YGGRND_B.SHP`). Before, the idle rollers drew nothing, and the grinding animation ended after about three frames, so the pit showed only its hole and pole. Nine animation types change: `YAGRND_A`, `YAGRND_AD`, `YAGRND_B`, `YAGRND_BD`, `YAGRND_D`, `YAGRND_DD`, `GAAIRCBB`, `GAREFNBB` and `NADEPTBB`. The last three are animations of other structures, which now draw where they drew nothing before.
