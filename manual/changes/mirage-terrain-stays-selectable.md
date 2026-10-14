---
title: Keep a vehicle that looks like terrain selectable
category: fix
release: 0.2.0
targets:
- type: system
  id: disguises
  effect: changed
credit:
- MentalHomiega
---

A vehicle that hides as terrain stays selectable while it looks like terrain to its owner. A click on it and a tap of the type-select key both reach it. Before this change, a vehicle drawn as terrain was missing from the clickable list, so a click on it could not select it, and a type-select tap passed it over until it was drawn as a tank again.
