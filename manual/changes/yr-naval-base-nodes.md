---
title: Stop the computer rebuilding naval structures it cannot place
category: fix
release: 0.2.0
targets:
- type: key
  id: Naval
  effect: added
credit: [Lucas]
---

A computer house that fails to place a `Naval=yes` structure now drops naval structures from its base plan, as Yuri's Revenge does, instead of starting the same structure again every frame.
