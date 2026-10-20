---
title: Draw animations in the layer their type names
category: fix
release: 0.2.0
targets:
- type: key
  id: Layer
  scope: animtype
  effect: changed
credit:
- MentalHomiega
---

An animation with [`Layer=top`](/keys/layer/#scope-animtype) is now drawn in the top layer with flying aircraft, not in the air layer below them. `Layer=surface` and `Layer=underground` now draw below the objects on the ground. `Layer=ground` and `Layer=air` are unchanged. Before, `top` drew in the air layer, and `surface` and `underground` drew in the ground layer. Saves made before this build do not load.
