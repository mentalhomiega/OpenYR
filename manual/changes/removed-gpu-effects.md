---
title: Remove the GPU picture effects
category: feature
release: 0.2.0
targets:
- type: key
  id: SoftShadows
  effect: removed
- type: key
  id: SoftShadowRadius
  effect: removed
- type: key
  id: Shockwaves
  effect: removed
- type: key
  id: Bloom
  effect: removed
- type: key
  id: BloomStrength
  effect: removed
- type: key
  id: WaterRipples
  effect: removed
- type: key
  id: WaterRippleStrength
  effect: removed
- type: key
  id: CloudShadows
  effect: removed
- type: key
  id: CloudShadowStrength
  effect: removed
- type: key
  id: Weather
  effect: removed
- type: key
  id: WeatherStrength
  effect: removed
- type: key
  id: IonCannon.Ripple
  effect: changed
credit:
- MentalHomiega
---

Soft shadows, shockwaves, glow, water ripples, cloud shadows and weather are gone from GPU drawing, with their switches on the Graphics page and their keys. A settings file that still holds `SoftShadows`, `SoftShadowRadius`, `Shockwaves`, `Bloom`, `BloomStrength`, `WaterRipples`, `WaterRippleStrength`, `CloudShadows`, `CloudShadowStrength`, `Weather` or `WeatherStrength` loads without a message: the game ignores those lines and leaves them in the file. `GpuRender=yes` and the GPU vehicle keys are unchanged. A warhead with [`IonCannon.Ripple`](/keys/ioncannon.ripple/) now shows the software shockwave while the GPU draws the map view, as it already did in software drawing.
