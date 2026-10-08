---
title: Give no kill credit for a DontScore victim
category: feature
release: 0.2.0
targets:
- type: key
  id: DontScore
  effect: added
- type: system
  id: veterancy
  effect: changed
credit: [MentalHomiega]
---

We now read `DontScore=` for every object type. A victim with `DontScore=yes` gives its killer no experience, bounty, score or kill count, and its house does not record the killer as the house that last hurt it, as in Yuri's Revenge. Before, we read the key nowhere, so such a victim credited its killer like any other. Triggers still run on its destruction, and its owner still counts the loss. The type data is now part of the save, so saved games from earlier builds no longer load.
