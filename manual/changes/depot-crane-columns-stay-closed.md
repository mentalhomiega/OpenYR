---
title: Keep vehicles out of a service depot's crane columns
category: fix
release: 0.2.0
targets:
- type: key
  id: NumberImpassableRows
  effect: changed
credit:
- MentalHomiega
---

A vehicle sent to a service depot no longer drives through the crane columns. It goes round them to the pad, as in Yuri's Revenge. Only a `UnitAbsorb=yes` structure still lets a vehicle on its Enter mission cross the blocked columns.
