---
key: MinDebris
scope: warheadtype
label: Least debris from a shot
see_also: [MaxDebris, DebrisTypes, DebrisMaximums]
when_omitted:
  kind: value
  value: "0"
---

On a warhead, `MinDebris` sets the least debris a shot throws where it detonates, when [`MaxDebris`](/keys/maxdebris/#scope-warheadtype) allows any. Above `0`, the count is drawn from `MinDebris` up to one less than `MaxDebris`. A value below `0` is read as `0`.

A `MaxDebris` below `MinDebris` is raised to `MinDebris`. When the two are equal, the count is drawn from one less than `MinDebris` up to `MinDebris`, so a shot can throw one piece fewer than `MinDebris`.
