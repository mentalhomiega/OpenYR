---
title: Draw sim decisions from the synchronized random stream
category: fix
release: 0.2.0
targets:
- type: system
  id: capture
  effect: changed
- type: system
  id: disguises
  effect: changed
credit:
- MentalHomiega
---

The AI capture roll, the Mirage disguise choice and the roll that lets a computer house see through a blinking Mirage drew from the random stream that voices, sounds and the audio mixer also advance. An infantryman's animation stage, picked when a randomized action starts, did the same. Those draws depend on timing, so two machines in one network game could make different capture and disguise decisions. These picks now draw from the scenario's random stream, which every machine in the match advances the same way. A save or recording made before this change may not play back the same way.
