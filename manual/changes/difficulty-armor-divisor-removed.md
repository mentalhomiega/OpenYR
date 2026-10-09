---
title: Stop the difficulty Armor= from dividing damage
category: fix
release: 0.2.0
targets:
- type: key
  id: Armor
  scope: difficulty-settings
  effect: changed
credit:
- MentalHomiega
---

Before, a difficulty section's `Armor=` divided the damage its houses took, so a computer house in a 1.2 section took 83 points from a 100-point hit. We now apply only the country's `Armor=`, so the same hit deals 100 points, and the difficulty figure has no effect on damage.
