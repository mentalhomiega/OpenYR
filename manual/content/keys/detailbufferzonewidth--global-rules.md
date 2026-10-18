---
key: DetailBufferZoneWidth
scope: global-rules
label: Frame rate recovery buffer
see_also: [DetailMinFrameRateNormal]
when_omitted:
  kind: value
  value: "5"
---

The number of frames per second the rate must gain above [`DetailMinFrameRateNormal`](/keys/detailminframeratenormal/#scope-global-rules) before animations hidden by a low frame rate are drawn again. With `0`, the rate needs only to reach the minimum. A negative value counts as `0`.

```ini title="rulesmd.ini"
[AudioVisual]
DetailBufferZoneWidth=5
```
