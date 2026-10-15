---
title: Let rearming aircraft take planned orders
category: feature
release: 0.2.0
targets:
- type: key
  id: AircraftWaypointReload
  effect: added
- type: system
  id: aircraft-operations
  effect: changed
credit: [MentalHomiega]
---

An armed aircraft with limited ammunition now takes planned orders in planning mode, and goes to a dock to rearm when it runs out. It resumes its plan after rearming, including an attack it was carrying out. [`AircraftWaypointReload`](/keys/aircraftwaypointreload/) turns this off, which restores the refusal of every aircraft.
