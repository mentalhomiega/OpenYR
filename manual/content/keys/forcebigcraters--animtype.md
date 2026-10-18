---
key: ForceBigCraters
scope: animtype
label: Animation multiple-cell crater
see_also: ["Crater", "Scorch", "Craters", "CraterLevel"]
when_omitted:
  kind: value
  value: "no"
---

The crater that an animation leaves is a multiple-cell crater whenever one fits the cell, whatever the size of the animation's artwork. A single-cell crater is used only when no multiple-cell crater fits.

The flag matters only on an animation that also sets [`Crater=yes`](/keys/crater/#scope-animtype). That page owns when the crater is laid and the Tiberium removed with it. A forced crater may also lie on a cell that a building stands on, which a normal crater may not.

The stock `NUKEBALL` animation sets this flag.
