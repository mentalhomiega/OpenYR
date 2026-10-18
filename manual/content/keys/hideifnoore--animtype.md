---
key: HideIfNoOre
scope: animtype
label: Animation hidden without ore
see_also: [OreTwinkle, Value]
when_omitted:
  kind: value
  value: "no"
---

With `HideIfNoOre=yes`, the animation is not drawn while the cell it sits on holds no ore. It keeps playing while hidden, so its loop carries on, and it is drawn again on the frame its cell holds ore.

The check runs every game frame. A cell holds ore when its overlay is a [Tiberium](/systems/tiberium/#cell-state) overlay whose type has a nonzero [`Value`](/keys/value/). A cell seeded by a blossom tree holds no ore for this key, because the tree sets no overlay. A position outside the playfield holds no ore either.

The stock `TWNK1` ore sparkle sets this flag. Its sparkles stop showing over harvested ground and return when the ore grows back.
