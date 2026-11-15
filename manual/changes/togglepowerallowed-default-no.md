---
title: Leave power mode out unless the rules allow it
category: fix
release: 0.2.0
targets:
- type: key
  id: TogglePowerAllowed
  effect: changed
- type: command
  id: TogglePower
  effect: changed
- type: system
  id: sidebar
  effect: changed
credit: [MentalHomiega]
---

`TogglePowerAllowed` now defaults to `no`, as in Ares and Yuri's Revenge, which has no power mode. Without `TogglePowerAllowed=yes` the modern sidebar has no power button, and the Power Mode command does nothing and is not listed in the keyboard settings.
