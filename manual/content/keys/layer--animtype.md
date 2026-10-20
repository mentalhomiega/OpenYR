---
key: Layer
scope: animtype
label: 'Draw layer'
see_also: [Surface, YSortAdjust]
when_omitted:
  kind: value
  value: air
---

Chooses the layer the animation is drawn in. The layers are drawn in this order: `underground`, `surface`, `ground`, `air`, then `top`. Only the `ground` layer is sorted, among the objects on the ground. An animation attached to an object is always in `ground`, whatever this key says.

The names are not case sensitive. A value that is not one of the five names leaves the choice to [`Surface`](/keys/surface/). When this key is omitted, `Surface` chooses between `ground` and `air`. When both keys are set to a recognised value, `Layer` wins.

```ini title="artmd.ini"
[MYANIM] ; example animation
Layer=ground
```
