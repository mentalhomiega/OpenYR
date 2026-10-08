---
key: AltCameo
summary: The SHP file drawn as a trainable object's sidebar cameo after the local player spies on a barracks or war factory.
see_also: [Cameo, "system:capture"]
when_omitted:
  kind: value
  value: ""
  note: No alternate cameo is loaded, so the object keeps its `Cameo=` file after a spy effect.
---

The value is a filename without its `.SHP` extension. It is read from the section of the object's [Image ID](/keys/image/), as [`Cameo=`](/keys/cameo/) is. A file that cannot be loaded leaves the alternate cameo unset.

Once the local player's house has [infiltrated](/systems/capture/#infiltrating-it) a barracks, the sidebar draws the alternate cameo of each `Trainable=yes` infantry type that has one. After a war factory, it does the same for each `Trainable=yes` unit and each `Trainable=yes` structure with an `UndeploysInto` type. Only the local player's sidebar is affected.
