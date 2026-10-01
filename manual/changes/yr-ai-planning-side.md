---
title: Keep computer base plans to their own side
category: feature
release: 0.2.0
targets:
- type: key
  id: AIBasePlanningSide
  effect: added
- type: system
  id: ai-base-building
  effect: changed
credit: [Lucas]
---

A computer house now passes over role structures meant for another side (`AIBasePlanningSide`) or barred to its country (`RequiredHouses`, `ForbiddenHouses`) when it plans its base. Yuri's Revenge lets every country own most structures, so a Soviet computer player built Allied barracks and refineries before.
