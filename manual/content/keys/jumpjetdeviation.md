---
key: JumpjetDeviation
summary: "How far a jumpjet of this type bobs while it flies."
see_also: [WobbleDeviation]
when_omitted:
  kind: value
  value: "[JumpjetControls] WobbleDeviation"
---

Sets how many leptons above and below its flight height a jumpjet of this type bobs, as [`WobbleDeviation`](/keys/wobbledeviation/) describes. [`JumpjetNoWobbles=yes`](/keys/jumpjetnowobbles/) stops the bobbing. A type without the key uses `WobbleDeviation` from `[JumpjetControls]`.

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetDeviation=1
```
