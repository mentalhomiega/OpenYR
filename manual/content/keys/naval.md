---
key: Naval
summary: Marks the type as naval, which decides the factories that build it.
see_also: ["system:production"]
when_omitted:
  kind: value
  value: "no"
---

A computer house that cannot place a finished `Naval=yes` structure abandons it and refunds its cost, like any structure that cannot be placed. From then on it removes every `Naval=yes` structure from its base plan as it reaches it, instead of trying to build it again. A structure it already owns is not affected.

```ini title="rulesmd.ini"
[MyShipyard] ; example BuildingType
Naval=yes
```

A `Naval=yes` VehicleType is built only at a `Naval=yes` structure, and every other type only at a structure without the flag. A factory structure therefore never offers or builds the other kind, for a human player or the computer.

The key is read for every type. On a structure with [`WeaponsFactory=yes`](/keys/weaponsfactory/), it also decides where a finished ship leaves: on a water cell, as [Leaving the factory](/systems/production/#leaving-the-factory) describes. Otherwise it has no effect. A naval unit moves as its [`Locomotor`](/keys/locomotor/) and [`SpeedType`](/keys/speedtype/) decide.
