---
title: Free player colors
category: feature
release: 0.2.0
targets:
- type: key
  id: CustomColor
  effect: added
- type: key
  id: Color
  effect: changed
credit:
- MentalHomiega
---

A player in a skirmish or network lobby can pick any color with a hue and saturation square and a brightness slider, or one of the presets. The stock color at the seat still decides the seat. A lobby refuses a pick that is too close to another player's color, where a free color is involved, when the colors look alike with normal, protanopic or deuteranopic vision. A `ColorRGB=` entry in a launch file gives a seat a free color. Saves store the color of each house, and the save revision moved to 70.
