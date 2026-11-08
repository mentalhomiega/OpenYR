---
title: A spy disguise fools a house when the copied house is its ally
category: fix
release: 0.2.0
targets:
- type: key
  id: AlliedDisguise
  effect: changed
credit:
- MentalHomiega
---

A spy disguised as a soldier of another house now fools a house that is an ally of that copied house, as in Yuri's Revenge. The attack cursor and the entry checks used the spy's own alliances, so they disagreed with the checks that decide whether units attack or crush it.
