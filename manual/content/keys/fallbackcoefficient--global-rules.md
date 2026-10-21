---
key: FallBackCoefficient
scope: global-rules
label: Fall back coefficient
summary: Scales how fast a vehicle held by a direct rocker link settles back toward level.
see_also: [DirectRocker, DirectRockingCoefficient, Pushy]
when_omitted:
  kind: value
  value: "1"
---

While a [`DirectRocker`](/keys/directrocker/) link holds a vehicle that is tipped less than 90 degrees, the vehicle's per-frame tilt rate falls by this value times 0.002 radians, which pulls the vehicle back toward level. Past 90 degrees the rate rises by 0.002 radians a frame instead, which tips the vehicle further over. A higher value settles the vehicle faster. `0` stops the settling, so a linked vehicle keeps tipping until it passes 180 degrees and is destroyed.

Set it in the `[AudioVisual]` section. The value below is an example.

```ini title="rules.ini"
[AudioVisual]
FallBackCoefficient=0.5 ; example value
```
