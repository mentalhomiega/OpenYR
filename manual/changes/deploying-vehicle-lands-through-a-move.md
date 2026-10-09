---
title: Keep a deploying vehicle landing when a move follows the deploy order
category: fix
release: 0.2.0
targets:
- type: key
  id: DeployToLand
  effect: changed
  scope: unittype
credit: [MentalHomiega]
---

A flying Siege Chopper that has been told to deploy lands before it deploys, and a move order given before it touches down does not cancel the landing: it lands at the end of that move. Before, the move order replaced the landing, and the chopper hovered where the move ended.
