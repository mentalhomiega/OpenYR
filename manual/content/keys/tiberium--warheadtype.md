---
key: Tiberium
scope: warheadtype
label: Sets Tiberium off
see_also: ["system:tiberium", "ChainReaction"]
when_omitted:
  kind: value
  value: "no"
---

An explosion from a `Tiberium=yes` warhead thins the Tiberium in every cell it reaches whose overlay is a [`ChainReaction=yes`](/keys/chainreaction/) Tiberium overlay, by one growth stage for every ten points of damage. Explosions from other warheads leave such a cell alone. No explosion sets the Tiberium off; [Tiberium damage](/systems/tiberium/#damage) covers what does.

The flag makes no difference to a `ChainReaction=yes` overlay that is not Tiberium; the [`ChainReaction`](/keys/chainreaction/) page covers that case.
