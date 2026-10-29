---
title: Fly off while climbing after take-off
category: fix
release: 0.2.0
targets:
- type: system
  id: aircraft-operations
  effect: changed
credit:
- MentalHomiega
---

An aircraft that takes off now picks up speed from the second frame while it climbs, as in Yuri's Revenge. Before, it rose straight up to half its [`FlightLevel`](/keys/flightlevel/) before it moved forward. It also takes new orders and starts its approach at once instead of after the climb.
