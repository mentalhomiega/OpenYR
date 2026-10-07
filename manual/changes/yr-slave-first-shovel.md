---
title: Give a slave its first shovelful at once
category: fix
release: 0.2.0
targets:
- type: key
  id: HarvestRate
  effect: changed
credit: [MentalHomiega]
---

A slave on ore now takes its first shovelful as soon as it arrives and each further one `HarvestRate=` frames later, so a slave with `Storage=4` fills in three delays rather than four. Before, it waited one delay before the first shovelful.
