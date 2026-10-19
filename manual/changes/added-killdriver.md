---
title: Take a vehicle's driver with KillDriver warheads
category: feature
release: 0.2.0
targets:
- type: key
  id: KillDriver
  effect: added
- type: key
  id: KillDriver.Owner
  effect: added
- type: key
  id: KillDriver.KillBelowPercent
  effect: added
- type: key
  id: KillDriver.Chance
  effect: added
- type: key
  id: KillDriver.RemoveVeterancy
  effect: added
- type: key
  id: ProtectedDriver
  effect: added
- type: key
  id: ProtectedDriver.MinHealth
  effect: added
- type: key
  id: VeteranAbilities
  effect: changed
- type: key
  id: EliteAbilities
  effect: changed
credit:
- MentalHomiega
---

A warhead section in `rules.ini` with `KillDriver=yes` takes the driver of a vehicle it hits instead of damaging it: the passengers climb out and the vehicle goes to the house `KillDriver.Owner` names. `KillDriver.KillBelowPercent`, `KillDriver.Chance` and `KillDriver.RemoveVeterancy` limit and extend it. A vehicle type protects its driver with `ProtectedDriver=yes` or `ProtectedDriver.MinHealth=`, and `VeteranAbilities=` and `EliteAbilities=` accept `PROTECTED_DRIVER`.
