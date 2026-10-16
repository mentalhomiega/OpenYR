---
title: Keep a cloaked attacker visible while its chosen weapon reaches the target
category: fix
release: 0.2.0
targets:
- type: system
  id: cloaking
  effect: changed
credit:
- MentalHomiega
---

An object no longer starts hiding while its target is within range of the weapon chosen for that target. Before, the range test used the primary weapon, so a Boomer attacking a land target with its cruise missiles, which reach farther than its torpedo, hid again between shots and uncloaked for the next one. It now stays visible until its target is out of reach of that weapon or dies.
