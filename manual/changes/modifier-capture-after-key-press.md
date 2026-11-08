---
title: Capture a modifier-only key on the first try after a key press
category: fix
release: 0.2.0
targets:
- type: format
  id: keyboard-ini
  effect: changed
credit: [MentalHomiega]
---

On the Controls page, holding Control and Shift and releasing them binds that chord on the first try after a plain key press, such as Backspace clearing a slot. Before, that first try was ignored and a second one was needed. A modifier held while the capture box lost the keyboard focus also no longer blocks the next modifier-only capture.
