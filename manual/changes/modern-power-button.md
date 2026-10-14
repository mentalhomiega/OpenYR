---
title: Toggle power down mode from the modern panel's power button
category: feature
release: 0.2.0
targets:
- type: command
  id: TogglePower
  effect: changed
- type: system
  id: sidebar
  effect: changed
credit:
- MentalHomiega
---

Before, the modern panel's power down button showed "Coming soon" in its tooltip and ignored clicks. Now a click turns power down mode on or off, as the `TogglePower` command does, and the button is lit while the mode is on.
