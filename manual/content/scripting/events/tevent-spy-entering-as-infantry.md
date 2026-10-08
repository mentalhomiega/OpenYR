---
type: event
id: TEVENT_SPY_ENTERING_AS_INFANTRY
title: Spy entering as Infantry...
summary: Satisfied when infantry disguised as the named infantry type enters the tagged cell.
valid_values:
  - The number of an infantry type, which is its position in the rules' `[InfantryTypes]` list, counted from zero. The event compares it with the infantry type the soldier is disguised as. The name written after the number is not compared.
caveats:
  - Only infantry satisfy this event. A disguised vehicle or aircraft that enters the cell does not.
  - The soldier must be disguised as the named type when it enters the cell. A soldier disguised as another type does not satisfy it.
  - The soldier must be disguised when it enters the cell. A hit that wounds it ends the disguise, unless its type is [`PermaDisguise=yes`](/keys/permadisguise/).
  - A soldier that is not disguised never satisfies the event, even when its own type is the named one.
  - A fully cloaked soldier crosses the cell without satisfying the event.
  - The event attaches to a cell. Attaching it to a structure or a unit leaves it unsatisfied.
  - A cell under a bridge offers the event only to a soldier on the bridge deck, as [Entered by...](/mapping/events/tevent-player-entered/) does.
related:
  - type: system
    id: trigger-springing
  - type: system
    id: disguises
  - type: event
    id: TEVENT_SPY_ENTERING_AS_HOUSE
  - type: event
    id: TEVENT_PLAYER_ENTERED
  - type: key
    id: PermaDisguise
  - type: format
    id: scenario-triggers
---

We offer this event to the cell's tag as a soldier enters the cell, after [Entered by...](/mapping/events/tevent-player-entered/) and [Entered or Overflown by...](/mapping/events/tevent-entered-or-overflown/) have been offered. The soldier is the object the tag is offered with.

The type is the one the soldier is disguised as. A spy that copies another soldier's look satisfies the event when the copied type is the named one.

On a persistent tag, a satisfied event is [remembered](/systems/trigger-springing/#remembering-a-satisfied-event).
