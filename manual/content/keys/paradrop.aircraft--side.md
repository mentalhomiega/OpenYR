---
key: ParaDrop.Aircraft
scope: side
label: Side paradrop aircraft
summary: "Names the aircraft that carries paratroopers."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "PDPLANE"
---

The aircraft type that carries the paratroopers of this side, unless the country or the weapon names one. This is an Ares key.

```ini title="rulesmd.ini"
[GDI] ; example side
ParaDrop.Aircraft=PDPLANE
```
