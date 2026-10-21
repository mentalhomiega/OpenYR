---
title: Pick a parasite bite animation by eight directions
category: fix
release: 0.2.0
targets:
- type: key
  id: Anim
  scope: weapontype
  effect: changed
- type: system
  id: parasites
  effect: changed
credit: [MentalHomiega]
---

A parasite's bite now picks its [`Anim`](/keys/anim/) entry by the victim's facing among eight directions, as Yuri's Revenge does, whatever the list's length. Before, an eight-entry list started at north-west, a list of 16, 32 or 64 entries picked among that many directions, and any other list longer than seven played its first entry. The first entry is now for north, and entries past the eighth never play. A list of fewer than eight entries still plays its first entry.
