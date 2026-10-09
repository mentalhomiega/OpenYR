---
title: Leave DontScore objects out of the owned count
category: fix
release: 0.2.0
targets:
- type: key
  id: DontScore
  effect: changed
credit: [MentalHomiega]
---

A house no longer counts an object of a type with `DontScore=yes` in its owned count for that type, as in Yuri's Revenge. Before, the owned count included such objects, such as `SLAV`, like any other.
