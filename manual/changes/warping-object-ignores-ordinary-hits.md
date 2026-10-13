---
title: Ignore ordinary hits on a frozen object
category: fix
release: 0.2.0
targets:
- type: system
  id: temporal-weapons
  effect: changed
credit:
- MentalHomiega
---

Before, a frozen object took ordinary hits and healing like any other object. Now an ordinary hit does nothing to it, and a Heal weapon does not heal it. A forced hit, such as the base heal crate's, still applies.
