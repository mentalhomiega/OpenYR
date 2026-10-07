---
title: Allow up to 100 local variables in a map
category: fix
release: 0.2.0
targets:
- type: system
  id: trigger-springing
  effect: changed
credit:
- MentalHomiega
---

A map may now declare up to 100 local variables in `[VariableNames]`, as in Yuri's Revenge. The engine kept only 50, so any trigger of Allied missions 1 to 3 that set or tested a variable numbered 50 or higher did nothing. A variable number outside the range is now skipped when the map is read instead of being written past the end of the list. Saves from older builds do not load.
