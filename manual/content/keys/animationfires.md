---
key: AnimationFires
summary: Whether Flamer and Scorch animations, and damaged or destroyed structures, light SmallFire and LargeFire fires.
see_also: [Flamer, Scorch, SmallFire, LargeFire, Sparky]
when_omitted:
  kind: value
  value: "no"
---

At `yes`, a [`Flamer`](/keys/flamer/) animation lights [`SmallFire`](/keys/smallfire/) and [`LargeFire`](/keys/largefire/) fires around itself, and a [`Scorch`](/keys/scorch/) animation lights a `SmallFire` at its position. Both happen at the animation's middle frame. The pages of those keys give the positions, timing and conditions.

At `yes`, a structure also lights these fires:

- A destroyed structure gives each cell of its footprint an even chance of a `SmallFire`, and a cell that gets one has a further even chance of a `LargeFire`.
- A structure knocked down a damage level by a warhead without [`Sparky=yes`](/keys/sparky/) gives each footprint cell an even chance of a `SmallFire`, unless an engineer caused the damage.

At `no`, none of these start a fire, and the destruction and damage rolls that decide them are not made. Yuri's Revenge reads the `Flamer` flag and starts no fire from it, starts none from `Scorch`, and keeps `SmallFire` and `LargeFire` unused otherwise, so `no` matches it.

This key does not change the damage fires that a hurt structure shows from its art `DamageFireOffset` points, the [`OnFire`](/keys/onfire/) flames of a `Sparky=yes` warhead, or the explosions a destroyed structure plays.

```ini title="rules.ini"
[AudioVisual]
AnimationFires=yes
```
