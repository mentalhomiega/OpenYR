---
key: Shadow
scope: animtype
label: 'Shadow frames'
see_also: [Layer]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the second half of the animation's file holds a shadow for each frame of the first half. The animation plays only the first half. Each frame is drawn first, then its shadow darkens whatever is beneath, including the frame itself. The shadow sits at the animation's position without the vertical offset that [`YDrawOffset`](/keys/ydrawoffset/) gives the frame. Without an `End`, the animation's length is half the file's frame count.

```ini title="artmd.ini"
[MYANIM] ; example animation
Shadow=yes
```

Tiled and [flat](/keys/flat/#scope-animtype) animations draw no shadow.
