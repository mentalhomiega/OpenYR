---
key: DetailMinFrameRateNormal
scope: global-rules
label: Minimum frame rate for detail-gated animations
see_also: [DetailBufferZoneWidth]
when_omitted:
  kind: value
  value: "15"
---

Animations whose [`DetailLevel`](/keys/detaillevel/#scope-animtype) is above `1` are not drawn while the frame rate is below this value. An animation whose `DetailLevel` is `1` or lower is drawn whatever the frame rate. The frame rate is the count the FPS readout shows, the number of frames counted in the last whole second.

Once the rate falls below this value, those animations stay hidden until the rate reaches this value plus [`DetailBufferZoneWidth`](/keys/detailbufferzonewidth/#scope-global-rules). A rate that reaches this value but not the sum keeps them hidden. The count starts at `0`, so they stay hidden until the first full second of play has been counted.

The test changes drawing only. A hidden animation is still created and advances through its frames.

A negative value counts as `0`, which disables the test.

```ini title="rulesmd.ini"
[AudioVisual]
DetailMinFrameRateNormal=15
```
