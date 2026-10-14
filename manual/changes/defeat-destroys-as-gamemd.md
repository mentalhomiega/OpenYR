---
title: Destroy a defeated house's objects as gamemd does
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_HOUSE_DESTROY_ALL
  effect: changed
- type: system
  id: observers
  effect: changed
- type: key
  id: HarvesterImmune
  effect: changed
- type: key
  id: C4Warhead
  effect: changed
credit:
- MentalHomiega
---

A defeated house's objects are destroyed in the pass gamemd uses. Each object the house originally owns takes one hit of its remaining strength from the C4 warhead, in the order the game keeps its objects, instead of repeated hits until it is gone. Passengers are hit in the same pass. An object that survives its hit is not hit again. Before, vehicles were destroyed first and then aircraft, structures and infantry.

The pass spares a unit that another house holds by mind control, and a unit the defeated house holds by mind control keeps its owner. Before, every object the house owned at the time was destroyed.

The Destroy all of... action runs the same pass before it marks the house defeated. The trigger actions that destroy buildings, land units or naval units run the pass narrowed to that kind, without a defeat.

A house flagged to die runs the pass when its borrowed time runs out, as gamemd does, rather than the blow-up it had before.

With the harvester truce on, the vehicles and ships a defeated house owns are removed as gamemd removes them. A unit the house holds by mind control is freed to its owner, its passengers are killed, and its bomb is disarmed before the removal. Before, the removal left those three things as they were.
