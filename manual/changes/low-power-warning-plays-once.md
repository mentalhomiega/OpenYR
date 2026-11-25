---
title: Play the low power warning once for each shortage
category: fix
release: 0.2.0
targets:
- type: system
  id: power
  effect: changed
- type: key
  id: SpeakDelay
  effect: changed
- type: key
  id: BuildConst
  effect: changed
credit:
- MentalHomiega
---

The low power warning plays when the player's house falls short of power and then stays quiet until the house has had full power again, as Yuri's Revenge does. Before, it repeated every `SpeakDelay` minutes for as long as the shortage lasted. The house must own one of the first three `BuildConst` structure types; later entries in the list no longer count.
