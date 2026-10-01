---
title: Show the Ivan bomb cursor
category: feature
release: 0.2.0
targets:
- type: system
  id: ivan-bombs
  effect: changed
- type: key
  id: Ivan
  effect: added
- type: key
  id: Bombable
  effect: added
credit: [Lucas]
---

An `Ivan=yes` soldier now shows the bomb cursor over targets it can bomb and refuses orders on `Bombable=no` targets and targets that already carry a bomb, as in Yuri's Revenge.
