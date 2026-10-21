---
key: DetailMinFrameRateNormal
scope: global-rules
label: Minimum frame rate for detail-gated effects
see_also: [DetailBufferZoneWidth]
when_omitted:
  kind: value
  value: "15"
---

Animations whose [`DetailLevel`](/keys/detaillevel/#scope-animtype) is above `1` are not drawn while the frame rate is below this value. An animation whose `DetailLevel` is `1` or lower is drawn whatever the frame rate. The frame rate is the count the FPS readout shows: the number of game frames counted in the last 960 milliseconds.

Once the rate falls below this value, those animations stay hidden until the rate reaches this value plus [`DetailBufferZoneWidth`](/keys/detailbufferzonewidth/#scope-global-rules). A rate that reaches this value but not the sum keeps them hidden. The count starts at `0`, so they stay hidden until the first 960 milliseconds of play have been counted.

The same test hides a particle whose type deals no damage. A combat flash is not made while the rate is low, or at detail level `0`, unless its warhead disables a color channel; a flash that disables one is still drawn. A spot light that disables no channel is not drawn while the rate is low. A laser beam is drawn as a plain shaded line while the rate is low.

Apart from combat flashes, the test changes drawing only. A hidden animation is still created and advances through its frames. A combat flash the test rules out is never created.

A negative value counts as `0`, which disables the test.

```ini title="rulesmd.ini"
[AudioVisual]
DetailMinFrameRateNormal=15
```
