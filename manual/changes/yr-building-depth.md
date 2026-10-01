---
title: Hide objects behind structures as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: key
  id: ZShapePointMove
  effect: changed
credit: [Lucas]
---

Structures now read their depth shape from Yuri's Revenge's reference point, so units behind a tall structure are hidden by it instead of drawn over it, and structures are no longer cut off. Only a structure eight or more cells wide is drawn without the depth shape, where six or more was before.
