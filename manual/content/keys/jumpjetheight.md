---
key: JumpjetHeight
summary: "The height a jumpjet of this type cruises at."
see_also: [CruiseHeight]
when_omitted:
  kind: value
  value: "[JumpjetControls] CruiseHeight"
---

Sets the height in leptons a jumpjet of this type climbs to and flies at, as [`CruiseHeight`](/keys/cruiseheight/) describes. A type without the key uses `CruiseHeight` from `[JumpjetControls]`.

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetHeight=500
```
