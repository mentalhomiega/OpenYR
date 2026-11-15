---
key: Damage.Delay
summary: "The animation frames between two applications of an animation's Damage."
see_also: [Damage, DamageRadius]
when_omitted:
  kind: value
  value: "0, so the damage applies on every animation frame"
---

An animation with [`Damage`](/keys/damage/#scope-animtype) above `0` damages what lies at its centre each time it advances a frame. With `Damage.Delay` above `1` it does so only on every `Damage.Delay`th frame of the animation, and the frames between do nothing. `0` and `1` apply it on every frame. The count is of animation frames, which pass more slowly than game frames when the animation has a long `Rate`.

```ini title="artmd.ini"
[MyFireAnim] ; example AnimType
Damage=1
Damage.Delay=4
```
