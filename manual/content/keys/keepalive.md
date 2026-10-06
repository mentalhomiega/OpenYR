---
key: KeepAlive
summary: Whether owning an object of this type keeps a player in a short game.
see_also: [BaseUnit, Insignificant]
when_omitted:
  kind: context-dependent
  note: A structure type that is not Insignificant=yes counts as yes; every other type counts as no.
---

With `KeepAlive=yes`, a player who owns a living object of this type is not defeated in a [short game](/formats/spawn-ini/). With `KeepAlive=no`, the object never keeps its owner in the game, even if it is a structure.

The key changes the short game only once some object type sets it, to either value. From then on a player is defeated when they own no object that keeps them alive and no vehicle of a [`BaseUnit`](/keys/baseunit/) type. A structure keeps its owner alive unless its type sets [`Insignificant=yes`](/keys/insignificant/) or `KeepAlive=no`. This includes a structure that can undeploy into a vehicle, which the `BaseUnit` page says the usual test does not count.

If no type sets `KeepAlive`, the short game keeps its usual test: a player loses when they own no structure and no `BaseUnit` vehicle. The long game, where a player loses with no objects left at all, is not affected.

```ini title="rulesmd.ini"
[MCV]    ; example VehicleType
KeepAlive=yes
[GAWALL] ; example BuildingType
KeepAlive=no
```

With this example, a player who owns only an MCV and `GAWALL` walls is still in the game, and a player with only walls is defeated.
