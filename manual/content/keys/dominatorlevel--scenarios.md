---
key: DominatorLevel
scope: scenarios
label: Scenario lighting
see_also: [Level, DominatorGround, "system:superweapons"]
when_omitted:
  kind: value
  value: "0"
---

While a [psychic dominator](/systems/superweapons/#psychic-dominator) blast lasts, `DominatorLevel` replaces [`Level`](/keys/level/#scope-scenarios) as the brightness each height level adds to a cell. The default `0` shades the map flat for the length of the blast. Aircraft in flight, and infantry and vehicles on bridges, also take their extra height brightness from this value. A lightning storm that is raging takes the place of this value.
