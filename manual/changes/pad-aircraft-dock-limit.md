---
title: Limit airport-bound aircraft to the docks of the helipads
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

An aircraft with `AirportBound=yes`, such as the Harrier and the Black Eagle, is refused in production once the pad aircraft reach the docks of the house's helipads. The pad aircraft are the types in `PadAircraft=`, counted in service plus those queued or under construction in the air factory. The docks are the `NumberOfDocks=` of every helipad the house owns, so one Air Force Command gives four. Before, these aircraft had no limit. `BuildLimit=` does not apply to them.
