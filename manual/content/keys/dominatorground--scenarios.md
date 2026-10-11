---
key: DominatorGround
scope: scenarios
label: Scenario lighting
see_also: [Ground, DominatorLevel, "system:superweapons"]
when_omitted:
  kind: value
  value: "0"
---

While a [psychic dominator](/systems/superweapons/#psychic-dominator) blast lasts, `DominatorGround` replaces [`Ground`](/keys/ground/) as the light subtracted from every cell. It uses the same scale, a fraction of full light. The default `0` darkens nothing. A lightning storm that is raging takes the place of this value.
