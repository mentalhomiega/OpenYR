---
key: AttachEffect.Animation
scope: warheadtype
label: Effect animation
when_omitted:
  kind: value
  value: "none"
  note: "No animation is shown."
---

`AttachEffect.Animation` names the animation shown on each object this warhead attaches its effect to. The animation moves with the object and repeats until the effect ends, whatever its own loop count. It is hidden while the object is off the map, cloaking or cloaked, and, with [`AttachEffect.TemporalHidesAnim=yes`](/keys/attacheffect.temporalhidesanim/#scope-warheadtype), while a temporal weapon warps the object out; it is shown again when that ends. With [`AttachEffect.AnimResetOnReapply=yes`](/keys/attacheffect.animresetonreapply/) a hit that restarts the effect also restarts the animation.

```ini title="rulesmd.ini"
[SlowGoo] ; example Warhead
AttachEffect.Duration=300
AttachEffect.Animation=TWLT100 ; plays on each object SlowGoo hits, for 300 frames
```
