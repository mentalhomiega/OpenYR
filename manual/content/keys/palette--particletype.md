---
key: Palette
scope: particletype
summary: "The palette file a gas, smoke or fire particle is drawn with."
see_also: [CustomPalette, AnimPalette]
when_omitted:
  kind: value
  value: "the animation palette"
---

The value is a palette file name with its `.pal`, such as `obliw.pal`, and three tildes in it stand for the theater's suffix. The key sets the colours of the particle's image for a `BehavesLike=Gas`, `Smoke` or `Fire` particle.

```ini title="rulesmd.ini"
[RuinerPart] ; example ParticleType
Palette=obliw.pal
```
