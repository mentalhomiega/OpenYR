---
key: ActiveAnimPoweredLight
summary: Whether the first active slot's animation is destroyed and recreated with its house's power.
see_also: ["ActiveAnim", "ActiveAnimPowered", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`ActiveAnim`](/keys/activeanim/) animation is removed while its house is short of power, and created again when the house has full power. The flag works only with [`ActiveAnimPowered=no`](/keys/activeanimpowered/) written beside it. While `ActiveAnimPowered` is `yes`, the animation freezes instead and this flag is ignored.

A shortfall removes the animation only on some structures; [Fields, fences and lights](/systems/power/#fields-fences-and-lights) says which.

Each time the structure comes into service, it creates the animation if the slot is empty, on a [`Powered=yes`](/keys/powered/) structure that drains power. A non-looping animation therefore plays again each time the structure comes back into service, even if no shortfall removed it.

Despite its name, the flag does not tint or light anything.
