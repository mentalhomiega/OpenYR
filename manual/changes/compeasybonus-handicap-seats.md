---
title: Apply the computer bonus to a seat's launch-file handicap
category: fix
release: 0.2.0
targets:
- type: key
  id: CompEasyBonus
  effect: changed
- type: format
  id: spawn-ini
  effect: changed
credit: [MentalHomiega]
---

A computer seat whose launch file sets its `[HouseHandicaps]` entry starts from that slot, and the multiplayer bonus then moves it down one slot as it does any other computer seat. Before, the entry skipped the bonus. The bonus still needs more than one human seat.
