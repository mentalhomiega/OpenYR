---
title: Give an object with no mission the settings gamemd gives it
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
breaking: false
credit:
- MentalHomiega
---

An object with no mission took its mission settings from memory outside the mission table, so a target scan could read a different threat flag on each machine of a network game. Such an object now takes the settings gamemd gives it. A target scan no longer skips it for its threat flag, which is off. Its delay between mission passes is zero, where the default gave a delay.

In a normal game, such an object is also no longer pulled off to defend a base, because gamemd does not recruit it for that. It still retaliates and scatters, and it is not a zombie or paralyzed.
