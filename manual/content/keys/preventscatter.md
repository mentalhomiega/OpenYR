---
key: PreventScatter
summary: "With yes, an object this warhead damages does not scatter because of the hit."
see_also: [PlayerScatter, "system:target-selection"]
when_omitted:
  kind: value
  value: "no"
---

An object hit by a `PreventScatter=yes` warhead stays where it is, even when [`PlayerScatter=yes`](/keys/playerscatter/) is set or the object holds the `SCATTER` ability. [Scattering after damage](/systems/target-selection/#scattering-after-damage) describes when an object scatters from a hit otherwise. Infantry that are frightened by a hit and flee on their own, such as civilians, still do.

```ini title="rulesmd.ini"
[MySuppressWH] ; example Warhead
PreventScatter=yes
```
