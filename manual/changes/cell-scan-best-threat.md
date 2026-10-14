---
title: Pick the highest-value threat in a cell scan
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
breaking: false
credit:
- MentalHomiega
---

A target scan that searches the cells around an object took the last candidate object it evaluated, even when an earlier candidate had a higher threat value. The scan now keeps the highest value, as gamemd's TechnoClass::GreatestThreat does. Air scans and the map-wide scan already kept the best value, so only the cell scan changes.
