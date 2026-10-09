---
key: WeaponsFactory
summary: Sends a finished object out through a door sequence instead of an ordinary exit cell.
see_also: ["system:production"]
when_omitted:
  kind: value
  value: "no"
---

A finished object appears at a fixed point inside the structure's footprint, not on an exit cell beside it. The structure then opens its door. The exit cell is the door cell inside the footprint: the eleventh cell of the structure's outline, one cell to the west, which for a `5x3` foundation is the east end of the middle row. While another object stands on the exit cell, that object and anything in the cells around the exit cell are told to move away. Once the exit cell is empty and the door is open, the finished object drives out, and the door closes behind it.

A weapons factory lets out one object at a time. While it is unloading, it hands the next object to another structure of the same type and house that is idle and producing nothing, and that structure lets the object out. If no such structure is free, the attempt is temporarily blocked. [Leaving the factory](/systems/production/#leaving-the-factory) covers what a blocked attempt does to the order.

A vehicle still in radio contact with a weapons factory starts only a queued move or enter order. Any other queued order waits until that contact ends.

A structure with [`Naval=yes`](/keys/naval/) does not use the door sequence for the ships it builds. It releases each ship onto a water cell.

A carryall cannot be ordered to pick up an object standing on such a structure.
