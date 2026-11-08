---
title: Speak once for a team or bulk selection and stay quiet for a type select
category: fix
release: 0.2.0
targets:
- type: key
  id: VoiceSelect
  effect: changed
credit: [MentalHomiega]
---

Selecting a team, or any command that selects many units at once, now gives one reply from the first unit selected, as in Yuri's Revenge. Selecting every unit of a type (the type select and select same type commands) gives no reply at all. Before, those commands made every unit answer together, and a team select left unit replies switched off until the next order or drag selection.
