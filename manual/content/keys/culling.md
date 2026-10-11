---
key: Culling
summary: Makes a giant squid's grapple sink a weakened victim instead of damaging it.
see_also: [Parasite, Naval, Organic, Trainable, "system:parasites"]
when_omitted:
  kind: value
  value: "no"
---

On the weapon of a giant squid, `Culling=yes` on the [`Parasite=yes`](/keys/parasite/) warhead makes each bite sink a weak victim instead of damaging it. A victim in the red health band is sunk. A victim in the yellow band is also sunk when the squid is elite. The health bands are the [`ConditionRed`](/keys/conditionred/) and [`ConditionYellow`](/keys/conditionyellow/) ratios of the victim's strength.

The squid comes out of the victim, and the victim sinks and dies without taking damage. A squid whose type is [`Trainable=yes`](/keys/trainable/) earns experience for the victim's cost, as for a kill. The squid is not paralyzed afterwards. If the victim belongs to the player, the unit-lost announcement plays.

Only a parasite whose type is both [`Naval=yes`](/keys/naval/) and [`Organic=yes`](/keys/organic/) reads the key. A victim that is not weak enough takes the bite as usual, and any other parasite ignores the key.

Stock rules set `Culling=yes` on `ParasitePlus`, the warhead of `SquidGrab` and `SquidGrabE`.

```ini title="rules.ini"
[MYGRAB] ; a WarheadType registered in [Warheads]
Parasite=yes
Culling=yes ; a weak victim sinks at the squid's next bite
```
