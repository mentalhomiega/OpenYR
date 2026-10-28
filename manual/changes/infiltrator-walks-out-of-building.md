---
title: Let an infiltrator that takes nothing walk out
category: fix
release: 0.2.0
targets:
- type: key
  id: Infiltrate
  effect: changed
- type: key
  id: C4
  effect: changed
credit:
- MentalHomiega
---

A soldier with [`Infiltrate=yes`](/keys/infiltrate/) that is neither an engineer nor an agent and enters the structure it was sent to capture now has its destination cleared and walks out, as in Yuri's Revenge. Before, it was removed. This covers a computer-owned `C4=yes` soldier sent against a structure it may not arm.
