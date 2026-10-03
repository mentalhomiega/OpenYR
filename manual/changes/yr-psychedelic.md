---
title: Drive units berzerk with Psychedelic warheads
category: feature
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
- type: system
  id: warheads
  effect: changed
- type: key
  id: Psychedelic
  effect: added
- type: key
  id: BerserkFriendly
  effect: added
- type: key
  id: BerzerkAllowed
  effect: changed
credit: [MentalHomiega]
---

A `Psychedelic=yes` warhead now drives vehicles, infantry and aircraft berzerk for a while, as the Yuri's Revenge Chaos Drone does. Every berzerk object now fires twice as fast, ignores its owner's orders, spares `BerserkFriendly=yes` types and never targets itself.
