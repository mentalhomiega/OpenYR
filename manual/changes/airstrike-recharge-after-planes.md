---
title: Start the airstrike recharge when the strike's last plane leaves
category: fix
release: 0.2.0
targets:
- type: key
  id: AirstrikeRechargeTime
  effect: changed
- type: key
  id: EliteAirstrikeRechargeTime
  effect: changed
- type: key
  id: Airstrike
  effect: changed
credit: [MentalHomiega]
---

The recharge of an airstrike now starts when the last of its planes leaves the map or is destroyed, as in Yuri's Revenge. Before, it started when the strike was called, so the next strike could come as soon as the planes were gone.
