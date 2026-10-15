---
title: Limit HDR output to a peak brightness
category: feature
release: 0.2.0
targets:
- type: key
  id: HDRPeakBrightness
  effect: added
credit: [MentalHomiega]
---

[`HDRPeakBrightness`](/keys/hdrpeakbrightness/) sets the highest brightness HDR output draws, in nits. It defaults to the peak the display reports, and the Peak brightness slider on the Display page changes it while the game runs. The game draws no values above SDR white yet, so the limit does not change the picture.
