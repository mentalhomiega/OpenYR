---
title: Hide a selling structure's animations and put out its fires
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

A structure that starts to be sold or undeployed now puts out its damage fires and hides its animations while its build-up plays backwards, as Yuri's Revenge does. Before, a damaged structure kept burning through its sale, and a refinery showed its ore-level animation over the shrinking building.
