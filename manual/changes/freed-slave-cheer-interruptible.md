---
title: Let a freed slave cheer only when its sequence can be interrupted
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit:
- MentalHomiega
---

A slave freed from its miner now cheers only when its current sequence can be interrupted, as in the original game. A slave that is lying down, getting up or in another sequence that cannot be interrupted keeps that sequence and does not cheer. It still guards and takes orders afterwards. Soldiers that cheer through the All to Cheer command are unchanged. No rules or art file needs a change.
