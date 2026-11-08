---
title: Play the buildup of a deployed MCV's structure
category: fix
release: 0.2.0
targets:
- type: key
  id: Buildup
  effect: changed
credit:
- MentalHomiega
---

A vehicle that deploys into a structure, such as an MCV into its Construction Yard, now plays the structure's [`Buildup=`](/keys/buildup/) animation to its end before the structure opens. The animation previously ended after one frame, so the Construction Yard appeared fully built.
