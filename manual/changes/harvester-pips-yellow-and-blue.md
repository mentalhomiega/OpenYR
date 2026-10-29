---
title: Draw Tiberium pips in yellow and blue, and give a slave pips
category: fix
release: 0.2.0
targets:
- type: key
  id: PipScale
  effect: changed
credit:
- MentalHomiega
---

A selected harvester now fills its pip row with gems in blue first, then ore in yellow, as in Yuri's Revenge. Before, the first Tiberium type was green and the others blue. A soldier with `PipScale=Tiberium`, such as a slave, now shows a row of three pips.
