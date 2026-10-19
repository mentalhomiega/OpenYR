---
title: Show the first frame of a custom insignia file
category: fix
release: 0.2.0
targets:
- type: key
  id: Insignia
  effect: changed
- type: key
  id: InsigniaFrame
  effect: changed
credit:
- MentalHomiega
---

A rank whose `Insignia` file is set and whose `InsigniaFrame` is not now draws frame 0 of that file, rookies included. Before, it drew the stock frame number of the rank, 14 or 15, which most single-frame insignia files lack, and a rookie drew nothing.
