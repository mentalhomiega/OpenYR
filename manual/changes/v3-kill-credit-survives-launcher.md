---
title: Credit a missile's kill to its house after the launcher dies
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit:
- MentalHomiega
---

A missile now credits its blast to the house that fired it when its launcher has been destroyed, so the kill counts for that house. Before, the kill depended on the launcher and was lost once the launcher died. The launcher still gets the experience only while it is alive.
