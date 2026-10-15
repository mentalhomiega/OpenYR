---
title: Refuse a map preview of an unreasonable size
category: fix
release: 0.2.0
targets:
- type: system
  id: loading-screens
  effect: changed
credit:
- MentalHomiega
---

A map whose preview size is not between 1 and 4096 pixels in each direction now shows no preview. Before, a size of tens of thousands of pixels made the game try to build a picture of gigabytes and fail.
