---
title: Offer the attacked events to a target of a mind control shot
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_ATTACKED_BY
  effect: changed
- type: event
  id: TEVENT_ATTACKED
  effect: changed
credit:
- MentalHomiega
---

A mind control shot now satisfies the Attacked by (house) and Attacked by any house events on the tag of the object it hits, before the capture is tried. Before, the shot raised neither event, so a trigger that waits for a house to attack an object by mind control never sprang. Red Alert 2 Soviet mission 9 wins this way: a Yuri Prime of the Africans must strike the President.
