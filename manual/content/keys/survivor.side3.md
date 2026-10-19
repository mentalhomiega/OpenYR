---
key: Survivor.Side3
summary: The infantry type that survives this object when its owner is on place 3 of [Sides].
see_also: ["Survivor.Side0", "Survivor.Side1", "Survivor.Side2", Crewed, AlliedCrew, SovietCrew, ThirdCrew]
when_omitted:
  kind: inherited
  note: The owning side's crew type, as AlliedCrew, SovietCrew or ThirdCrew gives it.
---

`Survivor.Side3` names the infantry type that a destroyed or sold object of this type releases when its owner's side holds place 3 in `[Sides]`, counting from `0`. The stock rules list the Allied side first, the Soviet side second and Yuri's side third. The type replaces the side's crew, and an owner on another place keeps its own.

A type named here is not replaced by a technician or an engineer, so [`Crew.TechnicianChance`](/keys/crew.technicianchance/) does not apply to it. `<none>` clears the key, so the side's crew applies.

The type is released only when the object's type is [`Crewed=yes`](/keys/crewed/). A name that is no infantry type is ignored. The keys cover the first four places of `[Sides]`.

```ini title="rulesmd.ini"
[MAD] ; example VehicleType
Crewed=yes
Survivor.Side3=TERROR ; a house on place 3 leaves a TERROR
```
