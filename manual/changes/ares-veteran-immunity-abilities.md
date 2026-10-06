---
title: Accept the Ares immunity abilities in VeteranAbilities and EliteAbilities
category: feature
release: 0.2.0
targets:
- type: key
  id: VeteranAbilities
  effect: changed
- type: key
  id: EliteAbilities
  effect: changed
credit:
- MentalHomiega
---

`VeteranAbilities` and `EliteAbilities` now accept the Ares values `EMPIMMUNE`, `RADIMMUNE`, `UNWARPABLE`, `POISONIMMUNE`, `PSIONICWEAPONIMMUNE` and `PSIONICSIMMUNE`. Each gives a veteran or elite object the immunity of the matching key, such as `ImmuneToPsionics=yes`. The Ares value `PROTECTED_DRIVER` is not read yet, since no weapon kills drivers.
