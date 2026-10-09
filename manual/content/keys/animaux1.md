---
key: AnimAux1
summary: The first frame, frame count and step delay of a structure's first auxiliary animation.
see_also: ["AnimIdle", "AnimActive", "AnimAux2"]
when_omitted:
  kind: value
  value: "0,1,0"
---

`AnimAux1` is the frame sequence a [`NukeSilo=yes`](/keys/nukesilo/) structure plays while it holds its door open for a launch. No other structure plays it. The value takes the same three numbers as [`AnimIdle`](/keys/animidle/): first frame, frame count and delay. The delay is used as written, with no game-speed adjustment. A damaged structure running this sequence draws damaged frames placed after the end of all four sequences, as `AnimIdle` describes.

A launch does not play this sequence. The missile leaves on the frame the launch starts, while [`AnimActive`](/keys/animactive/) is still opening the door, and the silo switches to [`AnimAux2`](/keys/animaux2/) on the next game frame.
