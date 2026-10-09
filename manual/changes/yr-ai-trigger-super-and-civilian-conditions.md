---
title: Let AI triggers test the iron curtain, chronosphere and civilian conditions
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-team-production
  effect: changed
credit: [MentalHomiega]
---

We had left condition types 5, 6 and 7 unimplemented, so an AI trigger with one of them never held and its teams were never raised. Types 5 and 6 now hold when the house's iron curtain or chronosphere is ready, or has charged at least `AIMinorSuperReadyPercent` of its recharge. Type 7 counts what the civilian house owns, as type 0 counts what the enemy owns. Yuri's Revenge's own AI rules contain 13 triggers of these types.
