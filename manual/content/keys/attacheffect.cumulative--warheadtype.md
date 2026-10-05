---
key: AttachEffect.Cumulative
scope: warheadtype
label: Stacking effects
see_also: ["system:attach-effects"]
when_omitted:
  kind: value
  value: "no"
---

`AttachEffect.Cumulative=yes` makes every hit of this warhead add a new copy of its effect, each with its own duration and animation, and their [multipliers combine](/systems/attach-effects/#multipliers). With `no`, an object carries at most one copy from this warhead, and a new hit restarts its duration.

```ini title="rulesmd.ini"
[Frenzy] ; example Warhead
AttachEffect.Duration=500
AttachEffect.Cumulative=yes
AttachEffect.FirepowerMultiplier=1.5 ; two hits give 2.25 times the damage
```
