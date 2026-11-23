---
title: Build Aircraft Type never fires
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_BUILD_AIRCRAFT
  effect: changed
credit: [MentalHomiega]
---

Build Aircraft Type... now needs an offer that names it, as in Yuri's Revenge, and nothing offers it, so the event is never satisfied. Before, it accepted any offer like the other three build events and fired once the house had produced the named aircraft.
