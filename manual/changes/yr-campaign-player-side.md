---
title: Play a campaign mission on the side its player country belongs to
category: fix
release: 0.2.0
targets:
- type: key
  id: ParentCountry
  effect: changed
credit: [MentalHomiega]
---

A campaign map whose player house names a map country, such as `[Player]` with `ParentCountry=Russians`, now loads that country's side before the mission starts, as in Yuri's Revenge. Before, the mission was set up on the Allied side: Soviet 2 and Soviet 7 showed the Allied sidebar and used the Allied speech and archives.
