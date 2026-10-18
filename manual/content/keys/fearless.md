---
key: Fearless
summary: Keeps the soldier's fear at zero, unless its type is also a Fraidycat.
see_also: [Fraidycat, Doggie, "system:veterancy"]
when_omitted:
  kind: value
  value: "no"
---

Fear is a figure from `0` to `300` that a soldier gains when it is shot at. A frightened soldier drops prone when it can. A [`Fraidycat=yes`](/keys/fraidycat/) soldier does not drop prone; it scatters instead. A fearless soldier's fear stays at zero, so neither happens, unless its type also sets `Fraidycat=yes`. That flag's panic ignores `Fearless`, as the caution below explains.

Three things normally raise fear, and none of them affects a fearless soldier:

- a hit from an attacker while its fear is below `100`, which raises it to `100`;
- any other hit, which adds `50`. If the soldier's health after the hit is above [`ConditionRed`](/keys/conditionred/), the amount is `25`. If it is above `ConditionYellow`, the amount is halved, which gives `12` with the stock thresholds;
- the [Panic](/mapping/missions/tmission-panic/) team mission, which raises it to `300`.

The `FEARLESS` [veteran ability](/systems/veterancy/#abilities) blocks the same three.

:::caution[`Fearless=yes` with `Fraidycat=yes` leaves a soldier frightened for good]
Two things normally lower fear: it falls by one point per logic frame, and the [Unpanic](/mapping/missions/tmission-unpanic/) team mission clears it. Neither happens for a `Fearless=yes` type. The veteran ability does not stop them.

This matters only when something else raised the fear. [A type that sets both](/keys/fraidycat/) is frightened by the first hit an attacker lands, or by firing its last round of ammunition, and never calms down.
:::
