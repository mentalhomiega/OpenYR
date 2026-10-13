---
title: Power output is one lower on an exact whole result
category: fix
release: 0.2.0
targets:
- type: system
  id: power
  effect: changed
credit:
- MentalHomiega
---

A structure whose scaled output comes out exactly whole now supplies one less when its strength fraction is not a binary fraction, as the original game's floating-point maths does. A `Power=200` plant at 15 of 750 strength supplies 3, not 4. At 300 of 750 it supplies 79, not 80.
