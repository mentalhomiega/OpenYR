---
title: Speak the announcer lines of Red Alert 2 missions
category: fix
release: 0.2.0
targets:
- type: format
  id: eva-ini
  effect: changed
- type: format
  id: opents-ini
  effect: changed
credit:
- MentalHomiega
---

The game now also reads Red Alert 2's announcer file, `EVA.INI`, and adds the lines `EVAMD.INI` does not list. Before, a Red Alert 2 mission that spoke one of those lines stayed silent, so Boot Camp had no drill sergeant. `RA2Eva=` under `[Files]` in `OPENTS.INI` names the file, and a name both files list keeps the `EVAMD.INI` line.
