---
title: Decloak an infantryman or structure that holds a target beyond range
category: fix
release: 0.2.0
targets:
- type: system
  id: cloaking
  effect: changed
credit:
- MentalHomiega
---

A cloaked infantryman now uncloaks once it holds a target, whatever the range, and it does so while it moves. Before, it stayed hidden until the target was in weapon range, and a moving one never uncloaked to fire. A cloaked structure that holds a target out of range now uncloaks and keeps the target; before, it dropped the target. A vehicle still uncloaks only within weapon range.
