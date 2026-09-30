---
key: NewTheater
scope: animtype
label: Theater animation naming
see_also: ["Theater", "Image"]
when_omitted:
  kind: value
  value: "no"
---

`NewTheater=yes` makes the animation draw a theater-specific shape when one exists. The file keeps its `.SHP` extension, and the second letter of its name is replaced by the scenario theater's [`ImageLetter`](/keys/imageletter/): `T` in temperate and `A` in snow. A name is rewritten only when it starts with `G`, `N`, `C` or `Y` and its second letter is `A` or `T`, ignoring case. `GACNST` follows this convention. `XTCNST` and `CITY01` do not, so each loads the same file in every theater.

The name rewritten is the one [`Image=`](/keys/image/#scope-animtype) gives, or the AnimType ID when no Image ID is set. [`Theater=yes`](/keys/theater/#scope-animtype) uses the AnimType ID instead, except in a scenario whose theater repeats the previous scenario's.

The renamed file is used when it exists. Otherwise the animation draws an unrenamed file: `<AnimType ID>.SHP` when that exists, and `<Image ID>.SHP` after that.

```ini title="art.ini"
[GAFLAG] ; example AnimType ID, with no Image= set
NewTheater=yes ; draws GTFLAG.SHP in temperate, or GAFLAG.SHP when that file is missing
```

Put `NewTheater=yes` in the art section of the Image ID, which is the AnimType's own section when `Image=` is not set. When the Image ID names another section, the AnimType's own section is read for this key only if neither `<AnimType ID>.SHP` nor `<Image ID>.SHP` exists.

:::caution[Some animations get no fallback]
Two kinds of animation draw nothing when the renamed file is missing, because the unrenamed file is never tried:

- a [`DemandLoad=yes`](/keys/demandload/#scope-animtype) animation;
- any animation after a saved game is loaded, even one that drew its unrenamed file before the save.
:::
