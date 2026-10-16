---
title: Let cloaking units on Guard pick targets
category: fix
release: 0.2.0
targets:
- type: system
  id: cloaking
  effect: changed
credit:
- MentalHomiega
---

A human player's cloaking vehicles, infantry, aircraft and submarines on the Guard mission now pick targets automatically. Before, they held fire, so a Typhoon sub on Guard did not attack an enemy sub a few cells away that its sensors showed. The unit still cloaks while idle. It uncloaks to fire with a weapon whose [`DecloakToFire`](/keys/decloaktofire--weapontype/) is `yes`.
