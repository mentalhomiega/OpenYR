---
title: Keep the keyboard on the capture box after a button click
category: fix
release: 0.2.0
targets:
- type: format
  id: keyboard-ini
  effect: changed
credit: [MentalHomiega]
---

On the Controls page, a click on a row, a trigger, a scheme, Assign, Clear, Swap, Cancel or a reset now leaves the keyboard on the capture box. Before, the click moved the keyboard to that button, so holding Ctrl and Shift after Assign showed nothing and that chord could not be bound. The bindings and the keyboard file are otherwise unchanged.
