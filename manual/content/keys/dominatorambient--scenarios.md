---
key: DominatorAmbient
scope: scenarios
label: Scenario lighting
see_also: [Ambient, DominatorRed, DominatorAmbientChangeRate, "system:superweapons"]
when_omitted:
  kind: value
  value: "1.5"
---

```ini title="map file"
[Lighting]
Ambient=1
DominatorAmbient=1.5
```

While a [psychic dominator](/systems/superweapons/#psychic-dominator) blast lasts, the map's light level moves toward this value in steps, and back to [`Ambient`](/keys/ambient/) when the blast is over. The scale is the one `Ambient` uses, where `1` is full daylight. The value is read to two decimal places. A lightning storm or a nuclear flash that is on screen takes the place of the dominator's lighting.
