---
key: ActiveAnimThreePoweredLight
summary: Whether the third active slot's animation is destroyed and recreated with its house's power.
see_also: ["ActiveAnimThree", "ActiveAnimThreePowered", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

`ActiveAnimThreePoweredLight=yes` removes the [`ActiveAnimThree`](/keys/activeanimthree/) animation when its structure drops out of service in a power shortfall, and creates it again when the structure comes back into service. Only some structures remove the animation; [Power](/systems/building-animations/#power) covers which. Despite its name, the flag does not light or tint anything.

The flag takes effect only beside [`ActiveAnimThreePowered=no`](/keys/activeanimthreepowered/). While `ActiveAnimThreePowered` is `yes`, its default, the animation freezes during a shortage and this flag is ignored.

Each time the structure comes into service, it creates the animation in the slot if the slot is empty, on a [`Powered=yes`](/keys/powered/) structure that drains power. That includes an animation that played to its end.

A repair step, or an [upgrade](/keys/upgrades/) installed on a structure below maximum strength, starts the animation if the slot is empty. Either can start it during a shortage, and it then runs until the structure next drops out of service.
