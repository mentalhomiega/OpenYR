---
title: Choose survivors per object type
category: feature
release: 0.2.0
targets:
- type: key
  id: Survivor.Pilots
  effect: added
- type: key
  id: Survivor.RookiePilotChance
  effect: added
- type: key
  id: Survivor.VeteranPilotChance
  effect: added
- type: key
  id: Survivor.ElitePilotChance
  effect: added
- type: key
  id: Survivor.RookiePassengerChance
  effect: added
- type: key
  id: Survivor.VeteranPassengerChance
  effect: added
- type: key
  id: Survivor.ElitePassengerChance
  effect: added
- type: key
  id: Survivor.Side0
  effect: added
- type: key
  id: Survivor.Side1
  effect: added
- type: key
  id: Survivor.Side2
  effect: added
- type: key
  id: Survivor.Side3
  effect: added
- type: key
  id: Crew.TechnicianChance
  effect: added
- type: key
  id: Crew.EngineerChance
  effect: added
credit:
- MentalHomiega
---

A type can name its own survivors. `Survivor.Pilots` and the per-rank `Survivor.RookiePilotChance`, `Survivor.VeteranPilotChance` and `Survivor.ElitePilotChance` set how many pilots a destroyed vehicle may leave and the odds of each; `Survivor.RookiePassengerChance`, `Survivor.VeteranPassengerChance` and `Survivor.ElitePassengerChance` set the odds that a passenger survives. `Survivor.Side0` to `Survivor.Side3` name the infantry type for an owner on that place in `[Sides]`, and `Crew.TechnicianChance` and `Crew.EngineerChance` set the odds that a survivor is a technician or an engineer. The keys follow the Ares documentation, and a type that sets none of them behaves as before.
