---
title: Keep CanPassiveAquire=no and CanRetaliate=no units from engaging on their own
category: feature
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
- type: key
  id: CanPassiveAquire
  effect: added
- type: key
  id: CanRetaliate
  effect: added
credit: [Lucas]
---

Types such as the Yuri's Revenge Demolition Truck and spy, which set `CanPassiveAquire=no` and `CanRetaliate=no`, no longer pick targets while idle or fire back when hit.
