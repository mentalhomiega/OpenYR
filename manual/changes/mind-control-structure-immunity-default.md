---
title: Keep structures without an ImmuneToPsionics key immune to mind control
category: fix
release: 0.2.0
targets:
- type: key
  id: ImmuneToPsionics
  effect: changed
- type: system
  id: mind-control
  effect: changed
credit: [MentalHomiega]
---

A structure whose rules entry has no `ImmuneToPsionics=` line is now immune, so neither mind control nor the psychic dominator takes it over. Before, such a structure could be taken over. A vehicle in a tank bunker can no longer be taken over either.
