---
title: Run Ares's trigger events 71 to 73, 75, 77 and 81 to 86
category: feature
release: 0.2.0
targets:
- type: event
  id: TEVENT_ENEMY_IN_SPOTLIGHT_REPEATING
  effect: changed
- type: event
  id: TEVENT_LIMPED
  effect: changed
- type: event
  id: TEVENT_ABDUCTED
  effect: added
- type: event
  id: TEVENT_ABDUCTED_BY_HOUSE
  effect: added
- type: event
  id: TEVENT_ABDUCTS_SOMETHING
  effect: added
- type: event
  id: TEVENT_ABDUCTS_SOMETHING_OF_HOUSE
  effect: added
- type: event
  id: TEVENT_ALL_KEEPALIVES_DESTROYED
  effect: added
- type: event
  id: TEVENT_ALL_KEEPALIVE_BUILDINGS_DESTROYED
  effect: added
- type: event
  id: TEVENT_ATTACKED_OR_DESTROYED_ANY
  effect: added
- type: event
  id: TEVENT_ATTACKED_OR_DESTROYED_BY
  effect: added
- type: event
  id: TEVENT_DESTROYED_BY_HOUSE
  effect: added
- type: event
  id: TEVENT_DRIVER_KILLED
  effect: added
- type: event
  id: TEVENT_DRIVER_KILLED_BY_HOUSE
  effect: added
- type: event
  id: TEVENT_HOUSE_DOES_NOT_OWN_TECHTYPE
  effect: added
- type: event
  id: TEVENT_HOUSE_OWNS_TECHTYPE
  effect: added
- type: event
  id: TEVENT_REMOVED_EMP
  effect: added
- type: event
  id: TEVENT_REMOVED_EMP_BY_HOUSE
  effect: added
- type: event
  id: TEVENT_REVERSE_ENGINEERED
  effect: added
- type: event
  id: TEVENT_REVERSE_ENGINEERS_ANYTHING
  effect: added
- type: event
  id: TEVENT_REVERSE_ENGINEERS_TYPE
  effect: added
- type: event
  id: TEVENT_SUPER_ACTIVATED
  effect: added
- type: event
  id: TEVENT_SUPER_DEACTIVATED
  effect: added
- type: event
  id: TEVENT_SUPER_NEAR_WAYPOINT
  effect: added
- type: event
  id: TEVENT_TECHTYPE_AT_MOST
  effect: added
- type: event
  id: TEVENT_UNDER_EMP_BY_HOUSE
  effect: added
- type: event
  id: TEVENT_VEHICLE_TAKEN
  effect: added
- type: event
  id: TEVENT_VEHICLE_TAKEN_BY_HOUSE
  effect: added
credit: [MentalHomiega]
---

A map's trigger events now use Ares's numbers 62 to 88. Events 71 to 73 (abduction), 75 (super weapon activated), 77 (super weapon activated near a waypoint), 81 and 82 (the trigger's house owns, or does not own, a number of a type), 83 and 84 (attacked or destroyed), 85 (destroyed by a house) and 86 (no more than a number of a type exist) fire as Ares documents them. The Ares events this engine does not run never fire. [`HouseDoesNotOwnAtMost`](/keys/housedoesnotownatmost/) chooses whether event 82 holds at the count it names.

Enemy In Spotlight... (repeating) moves from 63 to 66, which is Ares's number for the same event, and Limped moves from 64 to 89. Paralyzed stays at 62, Ares's Under EMP. A map that uses 63 or 64 for the old events must use the new numbers.
