---
key: PenetratesBunker
scope: warheadtype
label: 'Reaches the vehicle in a bunker'
see_also: [Bunker, "system:tank-bunkers"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the warhead damages a vehicle sitting in a bunker and does no damage to the bunker itself. Other warheads do no damage to a vehicle in a bunker. Damage dealt with defenses ignored is not affected.

```ini title="rulesmd.ini"
[MyWarhead] ; example Warhead
PenetratesBunker=yes
```
