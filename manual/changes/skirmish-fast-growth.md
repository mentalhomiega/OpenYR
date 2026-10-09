---
title: Keep a skirmish's growth speed out of its map
category: fix
release: 0.2.0
targets:
- type: key
  id: TiberiumGrows
  effect: changed
credit: [MentalHomiega]
---

A skirmish game no longer takes [`TiberiumGrows`](/keys/tiberiumgrows/#scope-scenarios) from the map's `[SpecialFlags]`. It keeps the growth speed the session already has, which is the full delay unless a network game earlier in the session turned fast growth on. Only a campaign mission reads the entry from its map, as in Yuri's Revenge.
