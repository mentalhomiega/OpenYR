---
title: Remove a cameo when its prerequisite is lost
category: fix
release: 0.2.0
targets:
- type: system
  id: sidebar
  effect: changed
- type: key
  id: RecheckPrerequisites
  effect: changed
credit:
- MentalHomiega
---

Losing a structure that another type names in `Prerequisite=`, such as a Battle Lab, now removes that type's cameo from the sidebar and cancels its production, as in Yuri's Revenge. Before, the cameo stayed and a click still started production. The behavior follows [`RecheckPrerequisites`](/keys/recheckprerequisites/), which defaults to `yes`; `RecheckPrerequisites=no` in `[General]` of `rules.ini` restores the old behavior.
