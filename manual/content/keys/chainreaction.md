---
key: ChainReaction
summary: Whether damage to the overlay's cell can set off the Tiberium standing there.
see_also: ["system:tiberium", "Tiberium", "Power"]
when_omitted:
  kind: value
  value: "no"
---

With `ChainReaction=yes`, a sonic wave passing over the overlay's cell can detonate the Tiberium there, and the detonation can spread to neighboring Tiberium cells. A destroyed Tiberium-spawning terrain object also sets off its cell. Explosions never detonate the cell.

An explosion that reaches the cell thins its Tiberium instead, by one growth stage for every ten points of its damage. On a [`Tiberium=yes`](/keys/tiberium/#scope-overlaytype) overlay, only an explosion whose warhead sets [`Tiberium=yes`](/keys/tiberium/#scope-warheadtype) does this. Without `ChainReaction=yes`, explosions never thin the cell. [Other effects](/systems/tiberium/#damage), such as a crater, can still remove stages.

[Tiberium damage](/systems/tiberium/#damage) gives the detonation chance, the damage it deals, and how it spreads.

A detonation needs Tiberium in the cell. On an overlay that is not Tiberium, the key has nothing to set off, because a cell holds only one overlay. The one exception is a cell that also holds a Tiberium-spawning terrain object, which counts as Tiberium of the type it spawns.

`ChainReaction` is separate from [`Explodes=yes`](/keys/explodes/#scope-overlaytype), which makes the overlay itself blow up.
