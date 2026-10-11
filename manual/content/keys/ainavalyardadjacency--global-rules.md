---
key: AINavalYardAdjacency
scope: global-rules
label: Naval yard distance from the yard
summary: The farthest distance, in cells, that a computer house may place a naval structure from its first construction yard.
see_also: [Shipyard, "system:ai-base-building"]
when_omitted:
  kind: value
  value: "20"
---

```ini title="rulesmd.ini"
[General]
AINavalYardAdjacency=20
```

A computer house places a naval structure on open water near its base, and drops the placement when the site lies farther than this many cells from the house's first construction yard. The distance is measured from the yard's center to the center of the site's first cell. A house with no construction yard skips the distance test. The [placement search](/systems/ai-base-building/#the-placement-search) describes how the site is found.
