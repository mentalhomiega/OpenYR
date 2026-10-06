---
title: Send armed aircraft that have fired back to rearm
category: fix
release: 0.2.0
targets:
- type: system
  id: aircraft-operations
  effect: changed
- type: key
  id: Dock
  effect: changed
credit: [MentalHomiega]
---

An armed aircraft on guard that has used any of its [`Ammo`](/keys/ammo/) now flies back to a [`Dock`](/keys/dock/) structure to reload, as in Yuri's Revenge. Before, only a computer-controlled aircraft with no ammunition left did so. A Harrier that fired one missile and was left idle by a player now returns to its airfield, and an aircraft already in radio contact with a structure is left alone.
