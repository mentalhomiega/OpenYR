---
title: Let any vehicle type set its repair order voice (VoiceIFVRepair)
category: feature
release: 0.2.0
targets:
- type: key
  id: VoiceIFVRepair
  scope: unittype
  effect: added
credit: [MentalHomiega]
---

A vehicle type with `VoiceIFVRepair` now answers an order its repair weapon would carry out with that voice, as Ares does; the IFV keeps the `[AudioVisual]` value when its type leaves the key out. Other types that leave the key out are unchanged.
