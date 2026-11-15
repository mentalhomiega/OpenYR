---
title: Look paradrop lists up by country, side and plane
category: feature
release: 0.2.0
targets:
- type: key
  id: ParaDrop.Types
  effect: changed
- type: key
  id: ParaDrop.Num
  effect: changed
credit: [MentalHomiega]
---

A paradrop weapon now reads the types, counts and aircraft of each plane in the order the Ares manual gives: `ParaDrop.<Country>.Plane<N>.*`, `ParaDrop.<Side>.Plane<N>.*` and `ParaDrop.Plane<N>.*` for planes after the first, then `ParaDrop.<Country>.*`, `ParaDrop.<Side>.*` and `ParaDrop.*`, and last the side's lists in `[General]`. Before, every plane carried the same load, one group was chosen for all of its keys, and a weapon that named only an aircraft ignored it. Saved games from before this change no longer load.
