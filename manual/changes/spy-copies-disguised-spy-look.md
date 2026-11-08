---
title: Make a spy that copies a disguised soldier wear that soldier's look
category: fix
release: 0.2.0
targets:
- type: system
  id: disguises
  effect: changed
credit:
- MentalHomiega
- Killerbote
---

A `CanDisguise=yes` object that hits a disguised soldier now takes on the look that soldier shows, not the soldier's own type. Before, a spy that copied another spy showed as a spy to enemy players. Now it shows the default soldier of the other spy's side, as the copied spy does. A soldier that is not disguised is copied as before, by its type and house.
