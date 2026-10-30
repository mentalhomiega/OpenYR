---
title: Show a factory's production cameo only to a spying country
category: fix
release: 0.2.0
targets:
- type: system
  id: observers
  effect: changed
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

An observer, or a defeated player given the whole map, no longer sees the cameo of what a selected factory is building unless its country has spied on that factory. Before, such a player saw it on every selected factory that was producing. A player who has not been given the whole map sees no change.
