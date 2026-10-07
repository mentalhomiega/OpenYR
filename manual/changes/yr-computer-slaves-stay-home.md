---
title: Keep slaves of a computer-controlled miner at work
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

A slave of a computer player no longer joins base defense, picks targets on its own, returns fire, or falls back to guarding the area when it stops. It waits at its miner for the miner's orders. Before, slaves of a computer-controlled Yuri left their ore field to fight and delivered little ore.
