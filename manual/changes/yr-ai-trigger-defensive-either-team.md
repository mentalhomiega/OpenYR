---
title: Treat an AI trigger as defensive when either of its teams is
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-team-production
  effect: changed
- type: key
  id: IsBaseDefense
  effect: changed
- type: key
  id: UseMinDefenseRule
  effect: changed
credit: [MentalHomiega]
---

An AI trigger now counts as defensive when its first or its second TeamType is `IsBaseDefense=yes`, as in Yuri's Revenge. Before, both had to be. Every defensive trigger of the Yuri side names a non-defensive second team, so a computer-controlled Yuri never held a defensive team. With `UseMinDefenseRule=yes` it could then spring no trigger at all, raised no teams and never attacked, and its credits piled up unspent.

A trigger whose current weight is exactly 5000 now takes priority in the draw: the house ignores every trigger that is not at 5000 while one at 5000 passes its gates.
