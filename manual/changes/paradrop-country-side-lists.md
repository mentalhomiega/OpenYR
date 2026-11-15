---
title: Read paradrop lists on countries and sides
category: feature
release: 0.2.0
targets:
- type: key
  id: ParaDrop.Types
  effect: added
  scope: housetype
- type: key
  id: ParaDrop.Num
  effect: added
  scope: housetype
- type: key
  id: ParaDrop.Aircraft
  effect: added
  scope: housetype
- type: key
  id: ParaDrop.Types
  effect: added
  scope: side
- type: key
  id: ParaDrop.Num
  effect: added
  scope: side
- type: key
  id: ParaDrop.Aircraft
  effect: added
  scope: side
credit: [MentalHomiega]
---

A country and a side can now carry their own `ParaDrop.Types`, `ParaDrop.Num` and `ParaDrop.Aircraft`. A paradrop plane that its weapon gives no list or aircraft takes them from the country, then the side, then the rules. A team with `Droppod=yes` flies in on the country's or side's aircraft. Saved games from before this change no longer load.
