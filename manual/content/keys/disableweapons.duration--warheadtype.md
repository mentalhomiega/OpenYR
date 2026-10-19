---
key: DisableWeapons.Duration
scope: warheadtype
summary: "Keeps the objects a warhead hits from firing for this many frames."
see_also: ["EMP.Duration", EffectsRequireVerses, EffectsRequireDamage]
when_omitted:
  kind: value
  value: "0, so the warhead jams nothing"
---

A warhead with a positive `DisableWeapons.Duration` stops each object its blast damages, and whose armor has a nonzero `Verses` against it, from firing for that many frames. A second hit sets the time to the longer of what is left and the new duration. The object still moves, reloads, picks targets and takes orders, and it fires again when the time ends. An object under the Iron Curtain is not jammed.

[`EffectsRequireVerses=no`](/keys/effectsrequireverses/) lets the jam reach a 0% armor, and [`EffectsRequireDamage=yes`](/keys/effectsrequiredamage/) holds it back from a hit that deals no damage.

```ini title="rulesmd.ini"
[MyJamWH] ; example Warhead
DisableWeapons.Duration=150  ; ten seconds
```
