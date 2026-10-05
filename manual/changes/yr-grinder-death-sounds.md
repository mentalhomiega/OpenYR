---
title: Play death sounds for ground-up objects
category: fix
release: 0.2.0
targets:
- type: key
  id: DieSound
  effect: changed
- type: key
  id: VoiceDie
  effect: changed
credit: [MentalHomiega]
---

An object ground up in a grinder made no death sound. It now plays its `VoiceDie` sound, heard only by its owner, and then its `DieSound`, as in Yuri's Revenge.
