---
title: Read hotkeys from KEYBOARDMD.INI
category: feature
release: 0.2.0
targets:
- type: format
  id: keyboard-ini
  effect: changed
- type: format
  id: opents-ini
  effect: changed
credit: [MentalHomiega]
---

Hotkeys are now read from and saved to `KEYBOARDMD.INI`, the file Yuri's Revenge uses, instead of `KEYBOARD.INI`. `Keyboard=` in the `[Files]` section of `OPENTS.INI` names a different file.
