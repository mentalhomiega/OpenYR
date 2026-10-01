---
key: Vertical
summary: "Makes this projectile fly in a straight line, with no gravity."
see_also: [DetonationAltitude, Acceleration, NukeMaker]
when_omitted:
  kind: value
  value: "no"
---

A `Vertical=yes` projectile keeps the direction it was launched in. Each frame it speeds up by its [`Acceleration`](/keys/acceleration/#scope-bullettype) until it reaches its weapon's speed. It explodes when it meets the ground, crosses a bridge deck, leaves the map, or climbs above [`DetonationAltitude`](/keys/detonationaltitude/).

```ini title="rulesmd.ini"
[MyNukeUp] ; example projectile
Vertical=yes
Acceleration=1
DetonationAltitude=20000
```

Set `DetonationAltitude` on every vertical projectile. At its default of `0`, the projectile explodes as soon as it rises above height zero.
