---
title: Start a skirmish with no computer player
category: feature
release: 0.2.0
targets:
- type: system
  id: skirmish-setup
  effect: changed
credit: [MentalHomiega]
---

The skirmish lobby refused to start until one computer player was added. A game whose other rows are all Open or Closed now starts with the person alone. It has no opponent to defeat, so it ends when the person's house is defeated, when the person quits, or when a map trigger ends it. Setups with computer players still need them not all on one team.
