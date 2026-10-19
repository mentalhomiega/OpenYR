---
title: Read more Phobos keys that Mental Omega uses
category: feature
release: 0.2.0
targets:
- type: key
  id: LowSelectionPriority
  effect: added
- type: key
  id: PrioritySelectionFiltering
  effect: added
- type: key
  id: Weapon
  effect: added
credit:
- MentalHomiega
---

A drag selection leaves out `LowSelectionPriority=yes` objects that it caught together with others, unless `PrioritySelectionFiltering=no` is set in the settings file. An animation fires a weapon with `Weapon=` in place of its `Damage` blast, and a terrain object is drawn through its own `Palette=`. The keys follow the Phobos documentation, and a type that sets none of them behaves as before.
