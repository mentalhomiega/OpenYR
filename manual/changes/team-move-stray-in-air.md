---
title: Let a team's airborne members stray twice as far while it moves
category: fix
release: 0.2.0
targets:
- type: key
  id: Stray
  effect: changed
- type: key
  id: TransportsReturnOnUnload
  effect: changed
- type: system
  id: ai-team-execution
  effect: changed
- type: mission
  id: TMISSION_MOVECELL
  effect: changed
- type: mission
  id: TMISSION_GATHER_AT_ENEMY
  effect: changed
- type: mission
  id: TMISSION_GATHER_AT_BASE
  effect: changed
credit:
- MentalHomiega
---

While a team moves, a member in the air may be twice `Stray` from the target before the team orders it on, and any other member, aircraft on the ground included, only `Stray`. Before this every aircraft had three times `Stray`.

A transport now has a return point only when a Load onto Transport line finished for it. Moving no longer records one.
