---
title: Build naval units only at naval factories
category: feature
release: 0.2.0
targets:
- type: key
  id: Naval
  effect: changed
- type: key
  id: Locomotor
  effect: changed
credit: [Lucas]
---

A `Naval=yes` VehicleType is now built only at a `Naval=yes` structure, and other types only at structures without the flag, as in Yuri's Revenge. The Ship locomotor, `{2BEA74E1-7CCA-11D3-BE14-00104B62A16C}`, is registered and moves like Drive; before, a type naming it crashed the game when created.
