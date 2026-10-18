---
key: AnimationFires
summary: Whether Flamer and Scorch animations light fires at their middle frame.
see_also: [Flamer, Scorch, SmallFire, LargeFire]
when_omitted:
  kind: value
  value: "no"
---

At `yes`, a [`Flamer`](/keys/flamer/) animation lights [`SmallFire`](/keys/smallfire/) and [`LargeFire`](/keys/largefire/) fires around itself, and a [`Scorch`](/keys/scorch/) animation lights a `SmallFire` at its position. Both happen at the animation's middle frame. The pages of those keys give the positions, timing and conditions.

At `no`, neither animation lights a fire. Yuri's Revenge reads the `Flamer` flag and starts no fire from it, and it starts no fire from `Scorch` either, so `no` matches it.

This key does not change the fires that a structure lights when it is damaged or destroyed.

```ini title="rules.ini"
[AudioVisual]
AnimationFires=yes
```
