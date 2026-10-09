---
title: Withhold disableable superweapons when the superweapons option is off
category: feature
release: 0.2.0
credit: [MentalHomiega]
targets:
- type: key
  id: DisableableFromShell
  effect: added
- type: format
  id: spawn-ini
  effect: changed
- type: system
  id: superweapons
  effect: changed
---

We read the superweapons option from the launch file's `SuperWeapons` key and from the skirmish lobby's Superweapons checkbox. Off, a superweapon whose `DisableableFromShell` is `yes` is withheld: its building cannot be built, the computer's base planning skips it, and no house is granted it. We withhold nothing while the option is on, which is the default.
