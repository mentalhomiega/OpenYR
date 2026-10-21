---
key: DoubleThick
scope: animtype
label: Animation fade under a temporal beam
see_also: ["Translucency", "Translucent", "system:temporal-weapons"]
when_omitted:
  kind: value
  value: "no"
---

With `DoubleThick=yes`, the animation is drawn three quarters faded while the structure that runs it is frozen by a [temporal weapon](/systems/temporal-weapons/). Without the flag it is drawn half faded.

The animation returns to normal when the structure is released. An animation that also fades by [`Translucent=yes`](/keys/translucent/#scope-animtype) or [`Translucency=`](/keys/translucency/#scope-animtype) adds that fade to this one. Without `DoubleThick=yes`, a 25 percent fade makes the animation three quarters faded, and a 50 or 75 percent fade is unchanged. With it, the animation is three quarters faded whatever its own fade.

```ini title="art.ini"
[MYVAT] ; a structure animation that fades deeply while the structure is frozen
Image=MYVAT
DoubleThick=yes
```
