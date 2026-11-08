---
title: Stop repair steps from starting building animations
category: fix
release: 0.2.0
targets:
- type: system
  id: building-animations
  effect: changed
- type: key
  id: ActiveAnim
  effect: changed
credit:
- MentalHomiega
---

A paid repair step no longer starts the active animations of a structure that has none running. It only switches the running animations between their healthy and damaged forms, as Yuri's Revenge does. A repaired prism tower no longer shows its idle animation over its charge animation, and a refinery under repair no longer starts the ore-level animations it does not show. A repair step also raises the structure's estimated health, which target selection reads, by the step it restores.
