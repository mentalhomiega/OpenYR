---
key: Layer
scope: animtype
label: 'Draw layer'
see_also: [Surface, YSortAdjust]
when_omitted:
  kind: value
  value: none
---

Chooses the layer the animation is drawn in, as [`Surface`](/keys/surface/) does. `ground`, `surface` and `underground` put it in the ground layer, sorted among the objects on the ground. `air` and `top` put it in the layer above, drawn after everything on the ground. Any other value leaves the choice to `Surface`. When both keys are set, `Layer` wins.

```ini title="artmd.ini"
[MYANIM] ; example animation
Layer=ground
```
