---
key: Image
scope: animtype
label: Animation Image ID
see_also: [End, Theater, NewTheater]
when_omitted:
  kind: computed
  note: Uses the AnimType ID as the animation Image ID.
---

The animation draws `<Image ID>.SHP` when that file exists. If it does not, the animation tries the same name with `G` as its second letter: when `YAGRND_B.SHP` is missing, `Image=YAGRND_B` draws `YGGRND_B.SHP`. If neither file exists, it draws nothing. An animation with [`DemandLoad=yes`](/keys/demandload/#scope-animtype) loads `<Image ID>.SHP` the first time it is needed and makes the same `G` retry.

An `Image=` that selects a different shape does not change the frame count; see [`End`](/keys/end/).

The theater settings treat this key differently:

- [`Theater=yes`](/keys/theater/#scope-animtype) names the artwork after the AnimType ID when the theater differs from the previous scenario's or a saved game is loaded. When the theater repeats, it uses the Image ID.
- [`NewTheater=yes`](/keys/newtheater/#scope-animtype) rewrites the theater letter in the name given here.
