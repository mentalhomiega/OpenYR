---
title: Keep slaves that go inside to unload
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
- type: key
  id: SlaveRegenRate
  effect: changed
credit: [MentalHomiega]
---

A slave that goes inside its miner to unload ore now rests there for `SlaveReloadRate` frames and comes out again. Before, the miner counted it as killed, made a replacement after `SlaveRegenRate` frames, and the slave that went inside never came out.
