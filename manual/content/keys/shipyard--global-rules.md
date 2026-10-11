---
key: Shipyard
scope: global-rules
label: Naval yard types for placement
summary: Lists the structure types whose foundation sets the open water a computer house looks for when it places a naval structure.
see_also: [AINavalYardAdjacency, "system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

```ini title="rulesmd.ini"
[General]
Shipyard=GAYARD,NAYARD,YAYARD
```

A comma-separated list of BuildingTypes. When a computer house places a naval structure, it uses the foundation of the first listed type its country may own, grown by one cell on every side, as the size of open water to find. With no usable entry, it uses the foundation of the structure being placed. The [placement search](/systems/ai-base-building/#the-placement-search) describes the rest.
