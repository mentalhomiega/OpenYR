---
key: MakeInfantryOwner
scope: animtype
summary: "The house that owns the soldier a MakeInfantry animation makes."
see_also: [MakeInfantry, InfDeathAnim, DeathAnims]
when_omitted:
  kind: value
  value: "invoker"
---

The value is `invoker`, `killer`, `victim`, `civilian`, `special`, `neutral` or `random`. `civilian`, `special` and `neutral` name the house of that name, and `random` picks any house in the game, including those three. The invoker depends on how the animation started: for an animation from a warhead's [`InfDeathAnim`](/keys/infdeathanim/) it is the owner of the killer, and for one from a soldier's `DeathAnims=` it is the owner of the soldier that died. Put the key on the animation that the death starts, and `MakeInfantry=` on the one that ends the chain.

```ini title="artmd.ini"
[NANODEATH1] ; example AnimationType
MakeInfantry=0
MakeInfantryOwner=victim
```
