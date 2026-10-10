---
key: Unsellable
summary: Prevents the player from selling the BuildingType's structures.
see_also: ["system:walls-and-gates"]
when_omitted:
  kind: value
  value: "no"
---

`Unsellable=yes` stops the player from selling any structure of this type. The sell cursor is never offered over it.

The flag does not stop sales the game starts itself. A computer house sells an unsellable structure when it sells a damaged one under its repair rules, and the [Sell building](/mapping/actions/taction-sell-attached/) trigger action sells it too. The base sale for money is the exception: it keeps the structure standing and removes its node. [Power and money interventions](/systems/ai-base-building/#power-and-money-interventions) describes that sale.

The flag also covers wall segments. A segment can be sold, or cleared for a new segment, gate or tower, only if the first BuildingType whose [`ToOverlay`](/keys/tooverlay/) lays its overlay is sellable. When several BuildingTypes lay the same overlay, only the one declared first counts. [Walls and gates](/systems/walls-and-gates/#crushing-clearing-and-selling) covers selling and clearing walls.
