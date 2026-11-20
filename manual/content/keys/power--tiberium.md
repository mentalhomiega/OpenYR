---
key: Power
scope: tiberium
label: Tiberium damage
when_omitted:
  kind: value
  value: "0"
  note: A damage figure of zero.
---

The value sets the damage this Tiberium deals in the two cases below. Standing on a cell of this type harms nothing.

With [`TiberiumExplosive=yes`](/keys/tiberiumexplosive/#scope-global-rules) in `[CombatDamage]`, a destroyed vehicle carrying Tiberium, such as a harvester, explodes through `C4Warhead`, which sets the blast's reach. The damage is the sum, over every Tiberium type it carries, of the amount carried multiplied by that type's `Power`. The blast goes off only if the vehicle has a death explosion animation, such as one from its [`Explosion`](/keys/explosion/) list. A scenario with [`HarvesterImmune=yes`](/keys/harvesterimmune/) prevents it. [Spilled harvester loads](/systems/destruction-and-debris/#spilled-harvester-loads) covers which deaths set it off.

A destroyed structure that is [`Explodes=yes`](/keys/explodes/#scope-aircrafttype), or has the explodes [veteran ability](/systems/veterancy/#abilities), adds the same sum for the Tiberium in its [`Storage`](/keys/storage/) to its [collateral figure](/keys/collateraldamagecoefficient/). That blast uses the warhead of the structure's primary weapon, not `C4Warhead`. A structure with no primary weapon deals no blast damage, so its stored Tiberium adds nothing.
