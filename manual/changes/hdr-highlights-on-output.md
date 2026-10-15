---
title: Draw highlights above paper white on HDR output
category: feature
release: 0.2.0
targets:
- type: key
  id: HDRHighlights
  effect: changed
- type: key
  id: HDROutput
  effect: changed
credit: [MentalHomiega]
---

Highlights now take effect on HDR output. A light source with an emissive mask is drawn above paper white, up to the peak brightness. The Highlights list on the Display page is enabled while the game shows HDR output, and its tooltip says so while it is grayed out.

The HDR output list is named Automatic, On and Off. On forces HDR10 output, and its tooltip warns that Windows HDR must be on for the display. The earlier "On (testing)" name is gone.
