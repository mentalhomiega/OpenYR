---
key: MaxDebris
scope: warheadtype
label: Debris from a shot
see_also: [MinDebris, DebrisTypes, DebrisMaximums, MetallicDebris, "system:destruction-and-debris"]
when_omitted:
  kind: value
  value: "0"
---

On a warhead, `MaxDebris` sets how much debris a shot throws where its bullet detonates. Above `0`, the count is drawn from [`MinDebris`](/keys/mindebris/) up to one less than `MaxDebris`; when the two are equal, the [`MinDebris` page](/keys/mindebris/#scope-warheadtype) gives the range. The pieces come from [`DebrisTypes`](/keys/debristypes/#scope-warheadtype) when that list is set. Otherwise they come from [`MetallicDebris`](/keys/metallicdebris/) animations, each 20 leptons above the bullet's position. At `0`, the shot throws none.

A shot that an Iron Curtain turns away throws none. A warhead with [`Airstrike=yes`](/keys/airstrike/) throws none either, because it calls its planes instead of detonating.

```ini title="rulesmd.ini"
[Smashing] ; a Warhead
MinDebris=1
MaxDebris=3
```
