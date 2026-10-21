---
title: Draw structure, parachute and grapple animations through Palette=
category: feature
release: 0.2.0
targets:
- type: key
  id: Palette
  effect: added
credit: [MentalHomiega]
---

A structure with `Palette=` in its art draws its cell-drawn animations and its destroy animation through that palette, in its house's colors. The Statue of Liberty sets `Palette=lib` in the stock art, so its destroy animation changes on a temperate map. Airstrike and Force Shield tints do not reach these animations.

A paradropped passenger's canopy and a giant squid's grapple take the `Palette=` of the vehicle, infantryman or aircraft they belong to, in its house's colors. Without one, they take their house's colors.
