---
title: Credit a house with kills that have no attacker
category: fix
release: 0.2.0
targets:
- type: system
  id: veterancy
  effect: changed
- type: system
  id: superweapons
  effect: changed
- type: system
  id: multiplayer-score-screen
  effect: changed
- type: event
  id: TEVENT_DISCOVERED
  effect: changed
- type: event
  id: TEVENT_ATTACKED
  effect: changed
- type: event
  id: TEVENT_DESTROYED
  effect: changed
- type: event
  id: TEVENT_DESTROYED_ANY
  effect: changed
- type: event
  id: TEVENT_DESTROYED_ANY_X
  effect: changed
credit:
- MentalHomiega
---

A super weapon that destroys an object with no attacker now credits the house that fired it, as gamemd does. This covers the lightning storm, the Iron Curtain's kill of infantry, the chronosphere's kill of organic units, the genetic mutator and the psychic dominator's blast. Before, none of these earned the firing house any score.

The house gets the victim's cost, doubled for a veteran and tripled for an elite, and nothing for an ally's object. No object gains experience and no bounty is paid. The dead object springs the attacked, discovered and destroyed-by-any-house events as it would for an attacker. A vehicle killed this way springs the attacked and discovered events only, and none of the three destroyed events, which is how gamemd behaves.

The Ivan bomb, the disk laser and a missile's blast still credit their attacker alone, so a dead planter or launcher is not credited through a house. The infantry mutation animation now takes its house from the damage itself.
