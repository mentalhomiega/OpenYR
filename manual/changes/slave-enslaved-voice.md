---
title: Play the enslaved voice lines for a Slave Miner's slaves
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

A slave that a Slave Miner owns now answers a select with its `VoiceSelectEnslaved` lines. Before, it used `VoiceSelect`, the list a freed slave uses. A freed slave still uses `VoiceSelect`. A type with no `VoiceSelectEnslaved` gives no select line while it is owned, as gamemd does.
