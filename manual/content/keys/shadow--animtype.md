---
key: Shadow
scope: animtype
label: 'Shadow frames'
see_also: [Layer]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the second half of the animation's file holds a shadow for each frame of the first half. The animation plays only the first half, and each frame is drawn over its shadow, which darkens the ground beneath. Without an `End`, the animation's length is half the file's frame count.

```ini title="artmd.ini"
[MYANIM] ; example animation
Shadow=yes
```

Tiled animations draw no shadow.
