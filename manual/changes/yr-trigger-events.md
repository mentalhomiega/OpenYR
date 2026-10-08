---
title: Check Yuri's Revenge's trigger events
category: feature
release: 0.2.0
targets:
- type: event
  id: TEVENT_BUILDING_DOES_NOT_EXIST
  effect: added
- type: event
  id: TEVENT_ENTERED_OR_OVERFLOWN
  effect: added
- type: event
  id: TEVENT_LAND_UNITS_DESTROYED
  effect: added
- type: event
  id: TEVENT_NAVAL_UNITS_DESTROYED
  effect: added
- type: event
  id: TEVENT_POWER_FULL
  effect: added
- type: event
  id: TEVENT_SPY_ENTERING_AS_HOUSE
  effect: added
- type: event
  id: TEVENT_SPY_ENTERING_AS_INFANTRY
  effect: added
- type: event
  id: TEVENT_TECHTYPE_DOES_NOT_EXIST
  effect: added
- type: event
  id: TEVENT_TECHTYPE_EXISTS
  effect: added
credit: [MentalHomiega]
---

Maps gain Yuri's Revenge's trigger events 53 to 61. Destroyed Units Naval and Land, Building does not exist, Power Full, Entered or Overflown By, TechType Exists and TechType does not Exist are checked; the two spy events are checked when a disguised soldier enters the tagged cell. An event that names an object type no longer takes the following event's place when a map is read.
