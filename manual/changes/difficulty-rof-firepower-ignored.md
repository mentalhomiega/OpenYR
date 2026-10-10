---
title: Stop the difficulty from changing damage and rate of fire
category: balance
release: 0.2.0
targets:
- type: key
  id: FirePower
  scope: difficulty-settings
  effect: changed
- type: key
  id: ROF
  scope: difficulty-settings
  effect: changed
- type: system
  id: difficulty
  effect: changed
credit:
- MentalHomiega
---

This intentionally differs from the original game. Yuri's Revenge multiplies a house's damage by its difficulty `FirePower=` and the delay between its shots by its difficulty `ROF=`, so a computer house reading the stock `[Easy]` section fires 20 percent more slowly than one reading `[Difficult]`. We now ignore both keys for human and computer houses, so the difficulty changes no unit's damage or rate of fire. The country `Firepower=` and `ROF=` still apply outside a campaign.
