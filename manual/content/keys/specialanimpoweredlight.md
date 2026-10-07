---
key: SpecialAnimPoweredLight
summary: Whether the first special slot's animation is destroyed and recreated with its house's power.
see_also: ["SpecialAnim", "SpecialAnimPowered", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SpecialAnim`](/keys/specialanim/) animation is removed while its house is short of power, and created again when the house has full power. The flag works only with [`SpecialAnimPowered=no`](/keys/specialanimpowered/) written beside it. While `SpecialAnimPowered` is `yes`, the animation freezes instead and this flag is ignored.

A shortfall removes the animation only on some structures; [Fields, fences and lights](/systems/power/#fields-fences-and-lights) says which.

Each time the structure comes into service, it creates the animation if the slot is empty, on a [`Powered=yes`](/keys/powered/) structure that drains power. This starts the slot on such a structure even when it is not a service depot, storage structure or firestorm wall section; [A powered light on a powered structure](/keys/specialanim/#a-powered-light-on-a-powered-structure) covers that route. A non-looping animation plays again each time the structure comes back into service.

The value is read only when the slot has an animation name from `SpecialAnim`, [`SpecialAnimDamaged`](/keys/specialanimdamaged/) or [`SpecialAnimGarrisoned`](/keys/specialanimgarrisoned/). Write it in the same art entry as the animation names. [Where each setting is read from](/systems/building-animations/#where-each-setting-is-read-from) has the full table.

Despite its name, the flag does not tint or light anything.
