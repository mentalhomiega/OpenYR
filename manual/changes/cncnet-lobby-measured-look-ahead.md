---
title: Set the look-ahead from the measured round trip in the built-in CnCNet lobby
category: feature
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

When the host of a built-in CnCNet room chooses a send period and no look-ahead, the look-ahead now comes from the measured round trip: the smallest whole number of send periods that covers the one-way trip, 50 ms for jitter and one send period. A 76 ms round trip at 60 frames a second and a send period of 3 now plays 9 frames instead of 15. The match also starts from the host's ping to the tunnel instead of a one-second look-ahead. Launch files carry the setting as `MeasuredMaxAhead` and `StartRoundTrip`; the stock CnCNet client and LAN games play as before.
