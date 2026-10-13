---
title: Start placing the finished building from the structure or defense tab
category: feature
release: 0.2.0
targets:
- type: command
  id: StructureTab
  effect: changed
- type: command
  id: DefenseTab
  effect: changed
credit: [MentalHomiega]
---

StructureTab and DefenseTab also start placing the building that finished last on that tab, as clicking its cameo does, when the house is not already placing a building. The tab holds it until it is placed, or until its factory no longer has it as a finished product. A map click that passes the proximity and shroud checks clears it, even if the game then refuses the placement.
