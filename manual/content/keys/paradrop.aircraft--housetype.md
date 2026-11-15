---
key: ParaDrop.Aircraft
scope: housetype
label: Country paradrop aircraft
summary: "Names the aircraft that carries paratroopers."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The country's side aircraft."
---

The aircraft type that carries this country's paratroopers, for paradrop weapons and for teams with `Droppod=yes` that a map action brings in. A weapon's own [`ParaDrop.Aircraft=`](/keys/paradrop.aircraft/) takes precedence. This is an Ares key.

```ini title="rulesmd.ini"
[Americans] ; example country
ParaDrop.Aircraft=PDPLANE
```
