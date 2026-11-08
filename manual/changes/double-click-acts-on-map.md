---
title: Act on the second click of a double click
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

A quick second click on the map now acts as a click. Before this change the map ignored it, so making a non-primary factory primary took three clicks: one to select the factory, a second that did nothing, and a third that set it primary. Now the second click sets it primary. The right button works the same way: its second press of a quick double click also acts, as in gamemd.
