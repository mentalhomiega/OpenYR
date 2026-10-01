---
key: Spread
scope: warheadtype
label: Damage falloff
see_also: ["system:emp-pulse"]
when_omitted:
  kind: value
  value: "1"
---

`Spread` no longer affects blast damage. [`CellSpread`](/keys/cellspread/) sets how far a blast reaches and [`PercentAtMax`](/keys/percentatmax/) how its damage thins with distance, as [How distance thins the damage](/systems/warheads/#how-distance-thins-the-damage) describes.

An [`EMEffect=yes`](/keys/emeffect/) warhead also uses this value as [the pulse radius in cells](/systems/emp-pulse/#firing-a-pulse), so a pulse's reach and its damage falloff cannot be set separately.

```ini title="rules.ini"
[MyPulseWH] ; a WarheadType with EMEffect=yes
Spread=4 ; the pulse reaches four cells out from its center
```

The computer also scales its rating of a weapon's strength against armor and against infantry by this value. At `Spread=0` both ratings are zero. A computer-built vehicle or infantryman whose primary weapon uses such a warhead, and cannot fire at aircraft, then waits near the center of its base instead of in one of the four zones around it.
