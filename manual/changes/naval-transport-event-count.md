---
title: Count a naval vehicle with passengers as a land unit in the destroyed-units events
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_LAND_UNITS_DESTROYED
  effect: changed
- type: event
  id: TEVENT_NAVAL_UNITS_DESTROYED
  effect: changed
credit:
- MentalHomiega
---

A naval vehicle that has passengers in its type's `Passengers=` capacity no longer counts as naval. "Destroyed, Units, Naval" no longer waits for it, and "Destroyed, Units, Land" waits for it instead. Yuri's Revenge counts a naval vehicle as naval only when its type has no passenger capacity. Naval vehicles without passengers count as before.
