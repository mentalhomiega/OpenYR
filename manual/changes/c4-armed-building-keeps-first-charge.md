---
title: Keep the first charge when a second saboteur reaches the structure
category: fix
release: 0.2.0
targets:
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

A saboteur that reaches a structure already carrying a charge no longer restarts the countdown or takes the credit for the detonation. It stops, rearms, and runs without uncloaking. A structure that is being sold or is iron-curtained still gets no charge.
