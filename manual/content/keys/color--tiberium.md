---
key: Color
scope: tiberium
label: Tiberium remap
see_also: ["system:tiberium", "Debris"]
when_omitted:
  kind: value
  value: "0"
  note: The scheme of the first entry in `[Colors]`.
---

The value names a color scheme declared in the rules' `[Colors]` section, in any letter case. A name that `[Colors]` does not declare leaves the type on the scheme it already had.

The scheme recolors two things:

- the [`CellAnim`](/keys/cellanim/) animation a Tiberium overlay of this type starts when it is placed;
- the [`Debris`](/keys/debris/) animation left when a [`TiberiumChainReaction=yes`](/keys/tiberiumchainreaction/) animation clears a cell of this type.

The overlays themselves are drawn in the theater's palette, as Yuri's Revenge draws ore and gems, so the scheme does not change them.
