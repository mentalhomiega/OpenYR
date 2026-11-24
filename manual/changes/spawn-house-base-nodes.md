---
title: Read computer base plans from the spawn house sections
category: feature
release: 0.2.0
targets:
- type: key
  id: UseMPAIBaseNodes
  effect: added
- type: system
  id: ai-base-building
  effect: changed
- type: key
  id: NodeCount
  effect: changed
- type: format
  id: scenario-objects
  effect: changed
credit:
- ZivDero
- Rampastring
- CCHyper
---

A skirmish or multiplayer map that sets `UseMPAIBaseNodes=yes` under `[Basic]` gives each house the base nodes written in the section of the start position it holds, `[Spawn1]` through `[Spawn8]`. A computer house builds those structures where they are written.

Only the plan changes: a computer house on such a map still inserts power plants, sells its base to raise money and replaces a lost base defense with a new pick, as in any skirmish. Maps without the key play as before.

Rampastring and CCHyper are credited for the Vinifera implementation this one follows.
