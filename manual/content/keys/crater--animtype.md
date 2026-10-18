---
key: Crater
scope: animtype
label: Animation crater
see_also: ["Scorch", "Flamer", "Craters", "CraterLevel"]
when_omitted:
  kind: value
  value: "no"
---

The animation leaves a crater where it plays, on its middle frame: the frame whose index is half the shape's frame count, rounded down. The count includes the shadow frames of a [`Shadow=yes`](/keys/shadow/#scope-animtype) animation. The crater appears only while the animation is less than 30 leptons above the ground. An animation whose middle frame is frame 0, such as one with a single frame, leaves its crater once, when it starts. Otherwise a looping animation repeats the attempt each time it reaches that frame, and a repeat on a cell that already holds a crater leaves no second one.

A [`Theater=yes`](/keys/theater/#scope-animtype) animation with no `Image=` key has no middle frame. Its crater is made as it starts, and its size is zero, so only single-cell craters fit it.

The crater is a random smudge type that declares itself a crater and fits the location. A crater fits only where its first cell, the upper-left one, lies inside the playfield and none of the following is true of any cell it would cover:

- the ground is a ramp
- the cell already has a smudge of any kind
- the cell holds an overlay
- a building stands on the cell
- the theater's tile does not allow marks on it

Craters that cover more than one cell are candidates only when the middle frame is more than 48 pixels wide and 40 pixels tall, or when no single-cell crater fits the location.

Each time the animation reaches that frame, it also removes Tiberium from the cell, as [Damage](/systems/tiberium/#damage) explains, whether or not a crater fits.

An animation that also sets [`Scorch=yes`](/keys/scorch/) leaves a scorch mark half the time and a crater the other half, never both. Tiberium is removed only on the crater half.

:::caution[A thrown animation rarely craters]
A [`Bouncer=yes`](/keys/bouncer/) or [`IsMeteor=yes`](/keys/ismeteor/#scope-animtype) animation skips the middle-frame test while it is in flight. It leaves a crater only when its middle frame is frame 0, and then at the point it was thrown from, not where it lands. The terrain a meteor impact deforms is a separate effect that [`CraterLevel`](/keys/craterlevel/) controls.
:::
