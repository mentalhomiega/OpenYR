---
key: ReverseEngineersVictims
summary: Makes this grinding structure teach its owner to build the types it grinds up.
see_also: [Grinding, CanBeReversed, ReversedAs, "system:production"]
when_omitted:
  kind: value
  value: "no"
---

A structure with `Grinding=yes` and `ReverseEngineersVictims=yes` teaches its owner the type of each infantryman or vehicle it grinds up, whichever side the type belongs to. From then on the owner can build that type without meeting its prerequisites, as [Reverse engineering](/systems/production/#reverse-engineering) describes. A victim whose type sets [`ReversedAs`](/keys/reversedas/) teaches that type instead, and one whose type sets [`CanBeReversed=no`](/keys/canbereversed/) teaches nothing.

The key has no effect without [`Grinding=yes`](/keys/grinding/). A type the owner already learned teaches nothing new. When the player's house learns a type, the announcer says `EVA_ReverseEngineeredInfantry` for infantry or `EVA_ReverseEngineeredVehicle` for a vehicle, then `EVA_NewTechnologyAcquired`. A name that [EVAMD.INI](/formats/eva-ini/) does not list is silent. The owner is still paid the refund.

```ini title="rulesmd.ini"
[MYGRINDER] ; example BuildingType
Grinding=yes
ReverseEngineersVictims=yes
```
