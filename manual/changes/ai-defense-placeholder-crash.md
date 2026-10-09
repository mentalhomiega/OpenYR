---
title: Stop a placeholder after a settled base defense from crashing
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

We fixed a crash in the computer's structure choice. After the planner settled a base defense, the next node was read as a structure type even when it was a `-1` or `-3` placeholder, which read the building table out of range. The next call now handles that placeholder.
