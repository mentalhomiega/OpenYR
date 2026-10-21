---
title: Make the firestorm wall animations optional
category: fix
release: 0.2.0
targets:
- type: key
  id: FirestormIdleAnim
  effect: changed
- type: key
  id: FirestormActiveAnim
  effect: changed
- type: key
  id: FirestormAirAnim
  effect: changed
- type: key
  id: FirestormGroundAnim
  effect: changed
credit:
- MentalHomiega
---

A rules file that names no firestorm wall animation no longer crashes the game when a raised wall needs one. The wall shows no animation, and the idle animation's random draw is skipped, as in Yuri's Revenge, which reads none of these keys.
