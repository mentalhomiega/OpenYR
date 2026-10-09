---
title: Keep campaign computers from turning paranoid
category: fix
release: 0.2.0
targets:
- type: key
  id: Paranoid
  effect: changed
- type: key
  id: MaxIQLevels
  effect: changed
credit: [MentalHomiega]
---

Before, a campaign mission made the computer houses paranoid when a human house allied with a house that is not `MultiplayPassive=yes`, and when a computer house at `MaxIQLevels` was defeated. Now only skirmish and multiplayer games do, as in gamemd. Recording playback is unchanged: it still hands a departing player's house to the computer and makes the computer houses paranoid.
