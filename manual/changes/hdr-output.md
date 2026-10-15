---
title: Send the picture to an HDR display as HDR10
category: feature
release: 0.2.0
targets:
- type: key
  id: HDROutput
  effect: added
credit: [MentalHomiega]
---

When Windows has HDR on for the game's display, the game now presents an HDR10 back buffer and draws its SDR art through a composite pass that keeps its colours. [`HDROutput`](/keys/hdroutput/) chooses Automatic, Off or On. Automatic is the default. Direct3D 11 is required; with another interface the picture stays SDR.
