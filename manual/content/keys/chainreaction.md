---
key: ChainReaction
summary: Whether an explosion that reaches the overlay's cell thins the Tiberium standing there.
see_also: ["system:tiberium", "Tiberium", "Power"]
when_omitted:
  kind: value
  value: "no"
---

A sonic wave passing over the overlay's cell does not set off the Tiberium there.

With `ChainReaction=yes`, an explosion that reaches the cell thins its Tiberium, by one growth stage for every ten points of its damage. On a [`Tiberium=yes`](/keys/tiberium/#scope-overlaytype) overlay, only an explosion whose warhead sets [`Tiberium=yes`](/keys/tiberium/#scope-warheadtype) does this. Without `ChainReaction=yes`, explosions never thin the cell. [Other effects](/systems/tiberium/#damage), such as a crater, can still remove stages.

`ChainReaction` is separate from [`Explodes=yes`](/keys/explodes/#scope-overlaytype), which makes the overlay itself blow up.
