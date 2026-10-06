---
title: List map packs and loose .yrm maps
category: feature
release: 0.2.0
targets:
- type: format
  id: map-packs
  effect: added
- type: key
  id: DescriptionText
  effect: added
- type: key
  id: Name
  scope: multiplayer-maps
  effect: changed
credit:
- MentalHomiega
---

Skirmish setup and the multiplayer map list now offer the maps in `.YRO` map packs and loose `.YRM` maps, as Yuri's Revenge does. They follow the packet-listed maps, and the `.MPR` maps still come last. A map pack's maps show their player limits after the name, such as "Circuit Board (2-4)".

A packet's map section can give its row text as written with `DescriptionText=`, in place of a string table label in `Description=`.

A loose map without `[Basic] Name=` is listed as "No Name". It used to take the name of the loose map listed before it.
