---
title: Explain refused keys and unbound commands on the keyboard page
category: fix
release: 0.2.0
targets:
- type: format
  id: keyboard-ini
  effect: changed
- type: command
  id: AttackMove
  effect: changed
credit: [MentalHomiega]
---

The keyboard page now says why it does not take a key press. Shift, Ctrl or Alt on its own, the left and right mouse buttons, and a key the page cannot name are refused with a notice, where before the press did nothing. Ctrl with Shift is already the attack-move chord, so the notice for Attack move says so.

Reset to defaults, Reset category and a scheme change now name each command that the new scheme gives no key. Attack move has no Classic key, so a Classic reset unbinds a Mouse 5 binding to it, and the notice says that. Bindings and the keyboard file are otherwise unchanged.
