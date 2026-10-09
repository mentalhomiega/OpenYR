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

We stopped a skirmish from reading [`TiberiumGrows`](/keys/tiberiumgrows/#scope-scenarios) from its map. A skirmish keeps the growth speed the session already has: the full delay, unless a network game earlier in the session turned fast growth on. Only a campaign mission reads the entry from its map, as in Yuri's Revenge.
