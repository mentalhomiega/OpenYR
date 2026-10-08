---
type: event
id: TEVENT_SPY_ENTERING_AS_HOUSE
title: Spy entering as House...
summary: Satisfied when infantry disguised as the named house enters the tagged cell.
valid_values:
  - A house number, as scenario trigger records define it. `50` to `57` name the start positions, as [Spawn houses](/formats/scenario-objects/#spawn-houses) explains. The event compares it with the house the soldier is disguised as.
caveats:
  - Only infantry satisfy this event. A disguised vehicle or aircraft that enters the cell does not.
  - The soldier must be disguised when it enters the cell. A hit that wounds it ends the disguise, unless its type is [`PermaDisguise=yes`](/keys/permadisguise/).
  - A soldier that is not disguised never satisfies the event, even when its own house is the named one.
  - A fully cloaked soldier crosses the cell without satisfying the event.
  - The event attaches to a cell. Attaching it to a structure or a unit leaves it unsatisfied.
  - A house number that names no house leaves the event unsatisfied.
  - A cell under a bridge offers the event only to a soldier on the bridge deck, as [Entered by...](/mapping/events/tevent-player-entered/) does.
related:
  - type: system
    id: trigger-springing
  - type: system
    id: disguises
  - type: event
    id: TEVENT_SPY_ENTERING_AS_INFANTRY
  - type: event
    id: TEVENT_PLAYER_ENTERED
  - type: key
    id: PermaDisguise
  - type: format
    id: scenario-triggers
---

We offer this event to the cell's tag as a soldier enters the cell, after [Entered by...](/mapping/events/tevent-player-entered/) and [Entered or Overflown by...](/mapping/events/tevent-entered-or-overflown/) have been offered. The soldier is the object the tag is offered with.

The house is the one the soldier is disguised as, not the house that owns it. A spy disguised as a soldier of an enemy house satisfies the event for that enemy house. A [`PermaDisguise=yes`](/keys/permadisguise/) soldier looks like its own side's default soldier, so it satisfies the event for its own house.

On a persistent tag, a satisfied event is [remembered](/systems/trigger-springing/#remembering-a-satisfied-event).
