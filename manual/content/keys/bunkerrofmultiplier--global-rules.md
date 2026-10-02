---
key: BunkerROFMultiplier
scope: global-rules
label: 'Bunkered vehicle rate of fire factor'
see_also: [BunkerDamageMultiplier, BunkerWeaponRangeBonus, "system:tank-bunkers"]
when_omitted:
  kind: value
  value: "1.0"
---

A vehicle in a bunker divides the delay after each burst by this value, so a value above `1.0` makes it fire faster. The result is rounded down to whole frames. `0` leaves the delay unchanged. The delays between shots of one burst are not changed.
